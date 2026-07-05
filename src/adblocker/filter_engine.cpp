#include "src/adblocker/filter_engine.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

#include "src/common/logging.h"
#include "src/common/string_utils.h"

// For URL downloading (platform-specific implementations would be needed)
#include <curl/curl.h>

namespace nullware {

// FilterRule implementation
bool FilterRule::Matches(const std::string& url, const std::string& source_url) const {
    // Skip exception rules for blocking check
    if (type == FilterType::EXCEPTION) {
        return false;
    }
    
    // Check domain restrictions
    std::string url_domain = ExtractDomain(url);
    std::string source_domain = ExtractDomain(source_url);
    
    // Check if domain matches restrictions
    if (!domain_restriction.empty()) {
        bool domain_match = false;
        
        // Check included domains
        if (!domains.empty()) {
            for (const auto& domain : domains) {
                if (EqualsIgnoreCase(url_domain, domain) || 
                    EndsWith(url_domain, "." + domain)) {
                    domain_match = true;
                    break;
                }
            }
        }
        
        // Check excluded domains
        if (domain_match) {
            for (const auto& domain : excluded_domains) {
                if (EqualsIgnoreCase(url_domain, domain) || 
                    EndsWith(url_domain, "." + domain)) {
                    domain_match = false;
                    break;
                }
            }
        }
        
        if (!domain_match) {
            return false;
        }
    }
    
    // Check third-party restriction
    if (third_party && !source_url.empty()) {
        if (EqualsIgnoreCase(url_domain, source_domain)) {
            return false;  // Same domain, not third-party
        }
    }
    
    // Match the pattern
    std::string normalized_url = NormalizeUrl(url);
    
    if (compiled_regex) {
        return std::regex_search(normalized_url, *compiled_regex);
    }
    
    // Fallback to wildcard matching
    return WildcardMatch(normalized_url, pattern);
}

std::unique_ptr<FilterRule> FilterRule::Parse(const std::string& rule_text) {
    auto rule = std::make_unique<FilterRule>();
    
    if (rule_text.empty() || rule_text[0] == '!') {
        return nullptr;  // Comment or empty line
    }
    
    std::string text = TrimString(rule_text);
    
    // Determine filter type
    if (StartsWith(text, "@@")) {
        rule->type = FilterType::EXCEPTION;
        text = text.substr(2);
    } else if (StartsWith(text, "##")) {
        rule->type = FilterType::ELEMENT_HIDING;
        text = text.substr(2);
    } else if (StartsWith(text, "#@#")) {
        rule->type = FilterType::CSS;
        text = text.substr(3);
    } else if (StartsWith(text, "#@%")) {
        rule->type = FilterType::SCRIPT_INJECTION;
        text = text.substr(3);
    } else {
        rule->type = FilterType::BLOCKING;
    }
    
    rule->pattern = text;
    rule->match_case = false;
    rule->third_party = false;
    rule->inline_script = false;
    rule->inline_font = false;
    rule->popup = false;
    rule->important = false;
    
    // Parse options (after $)
    size_t options_pos = text.find('$');
    if (options_pos != std::string::npos) {
        rule->pattern = TrimString(text.substr(0, options_pos));
        std::string options = text.substr(options_pos + 1);
        
        std::vector<std::string> option_parts = SplitString(options, ',');
        for (const auto& option : option_parts) {
            std::string opt = TrimString(ToLower(option));
            
            if (opt == "match-case") {
                rule->match_case = true;
            } else if (opt == "third-party") {
                rule->third_party = true;
            } else if (opt == "inline-script") {
                rule->inline_script = true;
            } else if (opt == "inline-font") {
                rule->inline_font = true;
            } else if (opt == "popup") {
                rule->popup = true;
            } else if (opt == "important") {
                rule->important = true;
            } else if (StartsWith(opt, "domain=")) {
                rule->domain_restriction = opt.substr(7);
                rule->domains = SplitString(rule->domain_restriction, '|');
            } else if (StartsWith(opt, "~domain=")) {
                std::string domains = opt.substr(9);
                rule->excluded_domains = SplitString(domains, '|');
            }
        }
    }
    
    // Compile regex pattern
    try {
        std::string regex_pattern = rule->pattern;
        
        // Convert EasyList pattern to regex
        // Escape special regex characters except * and ?
        regex_pattern = ReplaceAll(regex_pattern, ".", "\\.");
        regex_pattern = ReplaceAll(regex_pattern, "(", "\\(");
        regex_pattern = ReplaceAll(regex_pattern, ")", "\\)");
        regex_pattern = ReplaceAll(regex_pattern, "[", "\\[");
        regex_pattern = ReplaceAll(regex_pattern, "]", "\\]");
        regex_pattern = ReplaceAll(regex_pattern, "{", "\\{");
        regex_pattern = ReplaceAll(regex_pattern, "}", "\\}");
        regex_pattern = ReplaceAll(regex_pattern, "+", "\\+");
        regex_pattern = ReplaceAll(regex_pattern, "^", "\\^");
        regex_pattern = ReplaceAll(regex_pattern, "$", "\\$");
        regex_pattern = ReplaceAll(regex_pattern, "|", "\\|");
        regex_pattern = ReplaceAll(regex_pattern, "\\", "\\\\");
        
        // Convert EasyList wildcards to regex
        regex_pattern = ReplaceAll(regex_pattern, "*", ".*");
        regex_pattern = ReplaceAll(regex_pattern, "?", ".");
        
        // Anchor the pattern
        if (!StartsWith(regex_pattern, "^")) {
            regex_pattern = "^" + regex_pattern;
        }
        if (!EndsWith(regex_pattern, "$")) {
            regex_pattern = regex_pattern + "$";
        }
        
        // Create regex with case-insensitive flag if not match-case
        std::regex::flag_type flags = std::regex::ECMAScript;
        if (!rule->match_case) {
            flags |= std::regex::icase;
        }
        
        rule->compiled_regex = std::make_unique<std::regex>(regex_pattern, flags);
    } catch (const std::regex_error& e) {
        LOG_WARNING("Failed to compile regex for filter: " + rule->pattern + 
                   " - Error: " + std::string(e.what()));
        rule->compiled_regex = nullptr;
    }
    
    return rule;
}

// FilterEngine implementation
FilterEngine::FilterEngine() {
    // Initialize curl for downloading filter lists
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

FilterEngine::~FilterEngine() {
    Clear();
    curl_global_cleanup();
}

bool FilterEngine::LoadFilterListFromUrl(const std::string& url) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    LOG_INFO("Downloading filter list from: " + url);
    
    std::string content = DownloadUrl(url);
    if (content.empty()) {
        LOG_ERROR("Failed to download filter list from: " + url);
        return false;
    }
    
    // Extract name from URL
    std::string name = ExtractDomain(url);
    if (name.empty()) {
        name = "Custom List";
    }
    
    return LoadEasyList(content, name);
}

bool FilterEngine::LoadFilterListFromFile(const std::string& path) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    LOG_INFO("Loading filter list from file: " + path);
    
    std::string content = LoadFromFile(path);
    if (content.empty()) {
        LOG_ERROR("Failed to load filter list from file: " + path);
        return false;
    }
    
    std::string name = ExtractDomain(path);
    if (name.empty()) {
        name = path;
    }
    
    return LoadEasyList(content, name);
}

void FilterEngine::AddCustomFilter(const std::string& filter) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (custom_filters_.find(filter) != custom_filters_.end()) {
        return;  // Already exists
    }
    
    custom_filters_.insert(filter);
    
    // Parse and add to compiled rules
    auto rule = FilterRule::Parse(filter);
    if (rule) {
        compiled_rules_.push_back(std::move(rule));
    }
    
    LOG_DEBUG("Added custom filter: " + filter);
}

void FilterEngine::RemoveCustomFilter(const std::string& filter) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = custom_filters_.find(filter);
    if (it != custom_filters_.end()) {
        custom_filters_.erase(it);
        
        // Rebuild compiled rules
        CompileRules();
        
        LOG_DEBUG("Removed custom filter: " + filter);
    }
}

bool FilterEngine::Matches(const std::string& url, const std::string& source_url) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check exception rules first (they override blocking rules)
    if (MatchesExceptionRules(url, source_url)) {
        return false;
    }
    
    // Check blocking rules
    return MatchesBlockingRules(url, source_url);
}

const std::vector<std::unique_ptr<FilterList>>& FilterEngine::GetFilterLists() const {
    return filter_lists_;
}

const std::unordered_set<std::string>& FilterEngine::GetCustomFilters() const {
    return custom_filters_;
}

void FilterEngine::Clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    filter_lists_.clear();
    custom_filters_.clear();
    compiled_rules_.clear();
    domain_index_.clear();
    pattern_index_.clear();
    
    LOG_INFO("Filter engine cleared");
}

void FilterEngine::ReloadFilterLists() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    LOG_INFO("Reloading filter lists");
    
    // Save current filter lists
    std::vector<std::unique_ptr<FilterList>> old_lists = std::move(filter_lists_);
    
    // Clear current state
    Clear();
    
    // Reload each filter list
    for (auto& list : old_lists) {
        if (!list->url.empty()) {
            LoadFilterListFromUrl(list->url);
        } else if (!list->local_path.empty()) {
            LoadFilterListFromFile(list->local_path);
        }
    }
    
    // Re-add custom filters
    for (const auto& filter : custom_filters_) {
        AddCustomFilter(filter);
    }
    
    LOG_INFO("Filter lists reloaded");
}

size_t FilterEngine::GetRuleCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    size_t count = compiled_rules_.size();
    for (const auto& list : filter_lists_) {
        count += list->rules.size();
    }
    
    return count;
}

void FilterEngine::Optimize() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    LOG_INFO("Optimizing filter engine");
    
    // Build indexes
    domain_index_.clear();
    pattern_index_.clear();
    
    for (const auto& rule : compiled_rules_) {
        if (!rule->domain_restriction.empty()) {
            domain_index_.insert(rule->domain_restriction);
        }
        pattern_index_.insert(rule->pattern);
    }
    
    for (const auto& list : filter_lists_) {
        for (const auto& rule : list->rules) {
            if (!rule->domain_restriction.empty()) {
                domain_index_.insert(rule->domain_restriction);
            }
            pattern_index_.insert(rule->pattern);
        }
    }
    
    LOG_INFO("Filter engine optimization complete");
}

bool FilterEngine::LoadEasyList(const std::string& content, const std::string& name) {
    auto filter_list = std::make_unique<FilterList>();
    filter_list->name = name;
    filter_list->enabled = true;
    filter_list->last_updated = GetCurrentTimestamp();
    
    // Parse the content
    ParseEasyListRules(content, filter_list->rules);
    
    // Add to filter lists
    filter_lists_.push_back(std::move(filter_list));
    
    LOG_INFO("Loaded EasyList filter list: " + name + " with " + 
             std::to_string(filter_lists_.back()->rules.size()) + " rules");
    
    return true;
}

void FilterEngine::ParseEasyListRules(const std::string& content, 
                                      std::vector<std::unique_ptr<FilterRule>>& rules) {
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        line = TrimString(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '!' || line[0] == '[') {
            continue;
        }
        
        // Parse the rule
        auto rule = FilterRule::Parse(line);
        if (rule) {
            rules.push_back(std::move(rule));
        }
    }
}

std::string FilterEngine::DownloadUrl(const std::string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        return "";
    }
    
    std::string read_buffer;
    
    // Set URL
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    
    // Set timeout
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    
    // Set user agent
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "NullWare/" NULLWARE_VERSION_STRING);
    
    // Follow redirects
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    
    // Set write function
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, 
        [](void* contents, size_t size, size_t nmemb, void* userp) -> size_t {
            size_t realsize = size * nmemb;
            std::string* buffer = static_cast<std::string*>(userp);
            buffer->append(static_cast<char*>(contents), realsize);
            return realsize;
        });
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);
    
    // Perform the request
    CURLcode res = curl_easy_perform(curl);
    
    // Cleanup
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK) {
        LOG_ERROR("Failed to download URL: " + url + " - Error: " + 
                  std::string(curl_easy_strerror(res)));
        return "";
    }
    
    return read_buffer;
}

bool FilterEngine::SaveToFile(const std::string& path, const std::string& content) {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file.write(content.data(), content.size());
    return file.good();
}

std::string FilterEngine::LoadFromFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return "";
    }
    
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::string content(size, '\0');
    if (!file.read(&content[0], size)) {
        return "";
    }
    
    return content;
}

void FilterEngine::CompileRules() {
    compiled_rules_.clear();
    
    // Add rules from all filter lists
    for (const auto& list : filter_lists_) {
        if (list->enabled) {
            for (const auto& rule : list->rules) {
                compiled_rules_.push_back(rule->Clone());
            }
        }
    }
    
    // Add custom filters
    for (const auto& filter : custom_filters_) {
        auto rule = FilterRule::Parse(filter);
        if (rule) {
            compiled_rules_.push_back(std::move(rule));
        }
    }
    
    LOG_INFO("Compiled " + std::to_string(compiled_rules_.size()) + " rules");
}

bool FilterEngine::MatchesBlockingRules(const std::string& url, const std::string& source_url) const {
    for (const auto& rule : compiled_rules_) {
        if (rule->type == FilterType::BLOCKING && rule->Matches(url, source_url)) {
            return true;
        }
    }
    return false;
}

bool FilterEngine::MatchesExceptionRules(const std::string& url, const std::string& source_url) const {
    for (const auto& rule : compiled_rules_) {
        if (rule->type == FilterType::EXCEPTION && rule->Matches(url, source_url)) {
            return true;
        }
    }
    return false;
}

std::string FilterEngine::ExtractDomainForMatching(const std::string& url) const {
    std::string domain = ExtractDomain(url);
    return ToLower(domain);
}

std::string FilterEngine::NormalizeUrl(const std::string& url) const {
    std::string normalized = ToLower(url);
    
    // Remove protocol
    size_t scheme_end = normalized.find("://");
    if (scheme_end != std::string::npos) {
        normalized = normalized.substr(scheme_end + 3);
    }
    
    // Remove query and fragment
    size_t query_pos = normalized.find('?');
    if (query_pos != std::string::npos) {
        normalized = normalized.substr(0, query_pos);
    }
    
    size_t fragment_pos = normalized.find('#');
    if (fragment_pos != std::string::npos) {
        normalized = normalized.substr(0, fragment_pos);
    }
    
    // Remove trailing slash
    if (!normalized.empty() && normalized.back() == '/') {
        normalized.pop_back();
    }
    
    return normalized;
}

}  // namespace nullware
