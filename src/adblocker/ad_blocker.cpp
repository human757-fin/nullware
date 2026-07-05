#include "src/adblocker/ad_blocker.h"

#include "src/adblocker/filter_engine.h"
#include "src/adblocker/request_interceptor.h"
#include "src/common/logging.h"
#include "src/common/config.h"

namespace nullware {

AdBlocker::AdBlocker() : enabled_(true) {
    statistics_ = {0, 0, 0, 0};
}

AdBlocker::~AdBlocker() {
    Shutdown();
}

bool AdBlocker::Initialize() {
    LOG_INFO("Initializing AdBlocker");
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        filter_engine_ = std::make_unique<FilterEngine>();
        request_interceptor_ = std::make_unique<RequestInterceptor>(this);
        
        // Load default filter lists if enabled
        if (enabled_) {
            LoadDefaultFilterLists();
        }
        
        LOG_INFO("AdBlocker initialized successfully");
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to initialize AdBlocker: " + std::string(e.what()));
        return false;
    }
}

void AdBlocker::Shutdown() {
    LOG_INFO("Shutting down AdBlocker");
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (filter_engine_) {
        filter_engine_->Clear();
        filter_engine_.reset();
    }
    
    if (request_interceptor_) {
        request_interceptor_.reset();
    }
    
    LOG_INFO("AdBlocker shutdown complete");
}

bool AdBlocker::ShouldBlockUrl(const std::string& url, const std::string& source_url) {
    if (!enabled_ || !filter_engine_) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    statistics_.total_requests++;
    
    bool should_block = filter_engine_->Matches(url, source_url);
    
    if (should_block) {
        statistics_.blocked_requests++;
        LOG_DEBUG("Blocked URL: " + url);
    }
    
    return should_block;
}

bool AdBlocker::ShouldBlockRequest(const std::string& url, const std::string& method,
                                   const std::vector<std::pair<std::string, std::string>>& headers,
                                   const std::string& source_url) {
    // Check URL-based blocking first
    if (ShouldBlockUrl(url, source_url)) {
        return true;
    }
    
    // Additional request-based filtering can be added here
    // For example: block POST requests to tracking endpoints
    // Or block requests with specific headers
    
    return false;
}

RequestInterceptor* AdBlocker::GetRequestInterceptor() {
    return request_interceptor_.get();
}

void AdBlocker::SetEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    enabled_ = enabled;
    LOG_INFO("AdBlocker " + std::string(enabled ? "enabled" : "disabled"));
}

bool AdBlocker::IsEnabled() const {
    return enabled_;
}

bool AdBlocker::LoadFilterListFromUrl(const std::string& url) {
    if (!filter_engine_) {
        return false;
    }
    
    LOG_INFO("Loading filter list from URL: " + url);
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    bool success = filter_engine_->LoadFilterListFromUrl(url);
    if (success) {
        statistics_.filter_lists_loaded++;
    }
    
    return success;
}

bool AdBlocker::LoadFilterListFromFile(const std::string& path) {
    if (!filter_engine_) {
        return false;
    }
    
    LOG_INFO("Loading filter list from file: " + path);
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    bool success = filter_engine_->LoadFilterListFromFile(path);
    if (success) {
        statistics_.filter_lists_loaded++;
    }
    
    return success;
}

void AdBlocker::AddCustomFilter(const std::string& filter) {
    if (!filter_engine_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    filter_engine_->AddCustomFilter(filter);
    statistics_.filter_rules_loaded++;
}

void AdBlocker::RemoveCustomFilter(const std::string& filter) {
    if (!filter_engine_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    filter_engine_->RemoveCustomFilter(filter);
    statistics_.filter_rules_loaded--;
}

AdBlocker::Statistics AdBlocker::GetStatistics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return statistics_;
}

void AdBlocker::ResetStatistics() {
    std::lock_guard<std::mutex> lock(mutex_);
    statistics_ = {0, 0, 0, 0};
}

void AdBlocker::UpdateFilterLists() {
    if (!filter_engine_) {
        return;
    }
    
    LOG_INFO("Updating filter lists");
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Reload all filter lists
    filter_engine_->ReloadFilterLists();
    
    LOG_INFO("Filter lists updated");
}

void AdBlocker::LoadDefaultFilterLists() {
    if (!filter_engine_) {
        return;
    }
    
    LOG_INFO("Loading default filter lists");
    
    // Get filter list URLs from config
    const auto& config = ConfigManager::GetInstance().GetConfig();
    
    for (const auto& url : config.filter_list_urls) {
        LoadFilterListFromUrl(url);
    }
    
    // Load custom filter files
    for (const auto& file : config.custom_filter_files) {
        LoadFilterListFromFile(file);
    }
    
    LOG_INFO("Default filter lists loaded");
}

}  // namespace nullware
