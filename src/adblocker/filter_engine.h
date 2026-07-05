#ifndef NULLWARE_ADBLOCKER_FILTER_ENGINE_H_
#define NULLWARE_ADBLOCKER_FILTER_ENGINE_H_

#include <string>
#include <vector>
#include <memory>
#include <unordered_set>
#include <mutex>
#include <regex>

#include "src/common/string_utils.h"

namespace nullware {

// Filter types
enum class FilterType {
    BLOCKING,
    EXCEPTION,
    ELEMENT_HIDING,
    CSS,
    SCRIPT_INJECTION
};

// Filter rule structure
struct FilterRule {
    std::string pattern;
    FilterType type;
    std::string domain_restriction;
    std::vector<std::string> domains;
    std::vector<std::string> excluded_domains;
    std::vector<std::string> resource_types;
    bool match_case;
    bool third_party;
    bool inline_script;
    bool inline_font;
    bool popup;
    bool important;
    
    // Compiled regex for faster matching
    std::unique_ptr<std::regex> compiled_regex;
    
    // Check if this rule matches the given URL and source URL
    bool Matches(const std::string& url, const std::string& source_url) const;
    
    // Parse a filter rule from string
    static std::unique_ptr<FilterRule> Parse(const std::string& rule_text);
};

// Filter list structure
struct FilterList {
    std::string name;
    std::string url;
    std::string local_path;
    std::vector<std::unique_ptr<FilterRule>> rules;
    std::string last_updated;
    bool enabled;
};

// Filter engine for managing and matching filter rules
class FilterEngine {
public:
    FilterEngine();
    ~FilterEngine();
    
    // Load filter list from URL
    bool LoadFilterListFromUrl(const std::string& url);
    
    // Load filter list from file
    bool LoadFilterListFromFile(const std::string& path);
    
    // Add a custom filter rule
    void AddCustomFilter(const std::string& filter);
    
    // Remove a custom filter rule
    void RemoveCustomFilter(const std::string& filter);
    
    // Check if a URL matches any filter rule
    bool Matches(const std::string& url, const std::string& source_url = "") const;
    
    // Get all filter lists
    const std::vector<std::unique_ptr<FilterList>>& GetFilterLists() const;
    
    // Get custom filters
    const std::unordered_set<std::string>& GetCustomFilters() const;
    
    // Clear all filters
    void Clear();
    
    // Reload all filter lists
    void ReloadFilterLists();
    
    // Get the number of loaded rules
    size_t GetRuleCount() const;
    
    // Optimize the filter engine (build indexes, etc.)
    void Optimize();
    
private:
    mutable std::mutex mutex_;
    std::vector<std::unique_ptr<FilterList>> filter_lists_;
    std::unordered_set<std::string> custom_filters_;
    std::vector<std::unique_ptr<FilterRule>> compiled_rules_;
    
    // Indexes for faster lookup
    std::unordered_set<std::string> domain_index_;
    std::unordered_set<std::string> pattern_index_;
    
    // Load EasyList format filter list
    bool LoadEasyList(const std::string& content, const std::string& name);
    
    // Parse EasyList filter rules
    void ParseEasyListRules(const std::string& content, std::vector<std::unique_ptr<FilterRule>>& rules);
    
    // Download content from URL
    std::string DownloadUrl(const std::string& url);
    
    // Save content to file
    bool SaveToFile(const std::string& path, const std::string& content);
    
    // Load content from file
    std::string LoadFromFile(const std::string& path);
    
    // Compile all rules for faster matching
    void CompileRules();
    
    // Check if URL matches any blocking rule
    bool MatchesBlockingRules(const std::string& url, const std::string& source_url) const;
    
    // Check if URL matches any exception rule
    bool MatchesExceptionRules(const std::string& url, const std::string& source_url) const;
    
    // Extract domain from URL for matching
    std::string ExtractDomainForMatching(const std::string& url) const;
    
    // Normalize URL for matching
    std::string NormalizeUrl(const std::string& url) const;
};

}  // namespace nullware

#endif  // NULLWARE_ADBLOCKER_FILTER_ENGINE_H_
