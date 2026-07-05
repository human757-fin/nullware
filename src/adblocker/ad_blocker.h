#ifndef NULLWARE_ADBLOCKER_AD_BLOCKER_H_
#define NULLWARE_ADBLOCKER_AD_BLOCKER_H_

#include <string>
#include <vector>
#include <memory>
#include <unordered_set>
#include <mutex>

#include "src/common/config.h"

namespace nullware {

class FilterEngine;
class RequestInterceptor;

// Ad blocker interface
class AdBlocker {
public:
    AdBlocker();
    ~AdBlocker();
    
    // Initialize the ad blocker
    bool Initialize();
    
    // Shutdown the ad blocker
    void Shutdown();
    
    // Check if a URL should be blocked
    bool ShouldBlockUrl(const std::string& url, const std::string& source_url = "");
    
    // Check if a request should be blocked
    bool ShouldBlockRequest(const std::string& url, const std::string& method,
                           const std::vector<std::pair<std::string, std::string>>& headers,
                           const std::string& source_url = "");
    
    // Get the request interceptor for CEF integration
    RequestInterceptor* GetRequestInterceptor();
    
    // Enable or disable the ad blocker
    void SetEnabled(bool enabled);
    bool IsEnabled() const;
    
    // Load filter lists from URLs
    bool LoadFilterListFromUrl(const std::string& url);
    
    // Load filter lists from files
    bool LoadFilterListFromFile(const std::string& path);
    
    // Add custom filter rules
    void AddCustomFilter(const std::string& filter);
    void RemoveCustomFilter(const std::string& filter);
    
    // Get statistics
    struct Statistics {
        size_t total_requests;
        size_t blocked_requests;
        size_t filter_rules_loaded;
        size_t filter_lists_loaded;
    };
    
    Statistics GetStatistics() const;
    
    // Reset statistics
    void ResetStatistics();
    
    // Update filter lists
    void UpdateFilterLists();
    
private:
    std::unique_ptr<FilterEngine> filter_engine_;
    std::unique_ptr<RequestInterceptor> request_interceptor_;
    bool enabled_;
    Statistics statistics_;
    std::mutex mutex_;
    
    // Load default filter lists
    void LoadDefaultFilterLists();
};

// Global ad blocker instance
class AdBlockerManager {
public:
    static AdBlockerManager& GetInstance() {
        static AdBlockerManager instance;
        return instance;
    }
    
    AdBlocker* GetAdBlocker() { return &ad_blocker_; }
    const AdBlocker* GetAdBlocker() const { return &ad_blocker_; }
    
    void Initialize() { ad_blocker_.Initialize(); }
    void Shutdown() { ad_blocker_.Shutdown(); }
    
private:
    AdBlockerManager() = default;
    ~AdBlockerManager() = default;
    
    AdBlockerManager(const AdBlockerManager&) = delete;
    AdBlockerManager& operator=(const AdBlockerManager&) = delete;
    
    AdBlocker ad_blocker_;
};

}  // namespace nullware

#endif  // NULLWARE_ADBLOCKER_AD_BLOCKER_H_
