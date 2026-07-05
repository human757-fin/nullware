#include "src/browser/browser_main.h"

#include "src/common/logging.h"
#include "src/common/config.h"
#include "src/adblocker/ad_blocker.h"

#include <iostream>
#include <memory>

int main(int argc, char* argv[]) {
    // Initialize logging
    nullware::Logger::GetInstance().Initialize(
        nullware::LogLevel::INFO, 
        nullware::LogTarget::CONSOLE
    );
    
    LOG_INFO("NullWare Browser starting...");
    LOG_INFO("Version: " NULLWARE_VERSION_STRING);
    
    // Load configuration from command line
    nullware::ConfigManager::GetInstance().LoadFromCommandLine(argc, argv);
    
    // Get configuration
    const auto& config = nullware::ConfigManager::GetInstance().GetConfig();
    
    // Initialize ad-blocker if enabled
    if (config.enable_adblocker) {
        nullware::AdBlockerManager::GetInstance().Initialize();
        LOG_INFO("AdBlocker initialized");
    }
    
    // Initialize browser
    nullware::BrowserMain browser_main;
    if (!browser_main.Initialize(argc, argv)) {
        LOG_ERROR("Failed to initialize browser");
        return 1;
    }
    
    // Run browser
    int result = browser_main.Run();
    
    // Shutdown ad-blocker
    if (config.enable_adblocker) {
        nullware::AdBlockerManager::GetInstance().Shutdown();
        LOG_INFO("AdBlocker shutdown");
    }
    
    LOG_INFO("NullWare Browser exiting with code: " + std::to_string(result));
    
    return result;
}
