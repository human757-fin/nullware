#include "src/common/config.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>

#include "src/common/logging.h"
#include "src/common/string_utils.h"

namespace nullware {

void ConfigManager::UpdateConfig(const BrowserConfig& partial_config) {
    // Update ad-blocker settings
    if (partial_config.enable_adblocker != config_.enable_adblocker) {
        config_.enable_adblocker = partial_config.enable_adblocker;
    }
    
    // Update security settings
    if (partial_config.https_only_mode != config_.https_only_mode) {
        config_.https_only_mode = partial_config.https_only_mode;
    }
    if (partial_config.certificate_pinning != config_.certificate_pinning) {
        config_.certificate_pinning = partial_config.certificate_pinning;
    }
    if (partial_config.csp_enforcement != config_.csp_enforcement) {
        config_.csp_enforcement = partial_config.csp_enforcement;
    }
    if (partial_config.safe_browsing != config_.safe_browsing) {
        config_.safe_browsing = partial_config.safe_browsing;
    }
    if (partial_config.sandbox_enabled != config_.sandbox_enabled) {
        config_.sandbox_enabled = partial_config.sandbox_enabled;
    }
    
    // Update developer settings
    if (partial_config.enable_devtools != config_.enable_devtools) {
        config_.enable_devtools = partial_config.enable_devtools;
    }
    if (partial_config.remote_debugging != config_.remote_debugging) {
        config_.remote_debugging = partial_config.remote_debugging;
    }
    if (partial_config.devtools_port != config_.devtools_port) {
        config_.devtools_port = partial_config.devtools_port;
    }
    
    // Update paths
    if (!partial_config.user_data_dir.empty()) {
        config_.user_data_dir = partial_config.user_data_dir;
    }
    if (!partial_config.cache_dir.empty()) {
        config_.cache_dir = partial_config.cache_dir;
    }
    
    // Update proxy settings
    if (partial_config.use_proxy != config_.use_proxy ||
        partial_config.proxy_server != config_.proxy_server) {
        config_.use_proxy = partial_config.use_proxy;
        config_.proxy_server = partial_config.proxy_server;
    }
    
    // Update privacy settings
    if (partial_config.do_not_track != config_.do_not_track) {
        config_.do_not_track = partial_config.do_not_track;
    }
    if (partial_config.block_third_party_cookies != config_.block_third_party_cookies) {
        config_.block_third_party_cookies = partial_config.block_third_party_cookies;
    }
    
    // Update filter lists
    if (!partial_config.filter_list_urls.empty()) {
        config_.filter_list_urls = partial_config.filter_list_urls;
    }
    if (!partial_config.custom_filter_files.empty()) {
        config_.custom_filter_files = partial_config.custom_filter_files;
    }
}

void ConfigManager::LoadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        LOG(WARNING) << "Failed to open config file: " << path;
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Remove comments
        size_t comment_pos = line.find('#');
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }
        
        // Trim whitespace
        line = TrimString(line);
        if (line.empty()) continue;
        
        // Parse key-value pairs
        size_t separator_pos = line.find('=');
        if (separator_pos != std::string::npos) {
            std::string key = TrimString(line.substr(0, separator_pos));
            std::string value = TrimString(line.substr(separator_pos + 1));
            
            if (key == "enable_adblocker") {
                config_.enable_adblocker = (value == "true" || value == "1");
            } else if (key == "https_only_mode") {
                config_.https_only_mode = (value == "true" || value == "1");
            } else if (key == "certificate_pinning") {
                config_.certificate_pinning = (value == "true" || value == "1");
            } else if (key == "csp_enforcement") {
                config_.csp_enforcement = (value == "true" || value == "1");
            } else if (key == "safe_browsing") {
                config_.safe_browsing = (value == "true" || value == "1");
            } else if (key == "sandbox_enabled") {
                config_.sandbox_enabled = (value == "true" || value == "1");
            } else if (key == "enable_devtools") {
                config_.enable_devtools = (value == "true" || value == "1");
            } else if (key == "remote_debugging") {
                config_.remote_debugging = (value == "true" || value == "1");
            } else if (key == "devtools_port") {
                config_.devtools_port = std::stoi(value);
            } else if (key == "user_data_dir") {
                config_.user_data_dir = value;
            } else if (key == "cache_dir") {
                config_.cache_dir = value;
            } else if (key == "proxy_server") {
                config_.proxy_server = value;
                config_.use_proxy = !value.empty();
            } else if (key == "do_not_track") {
                config_.do_not_track = (value == "true" || value == "1");
            } else if (key == "block_third_party_cookies") {
                config_.block_third_party_cookies = (value == "true" || value == "1");
            } else if (key == "default_homepage") {
                config_.default_homepage = value;
            }
        }
    }
    
    LOG(INFO) << "Loaded configuration from: " << path;
}

void ConfigManager::SaveToFile(const std::string& path) const {
    std::ofstream file(path);
    if (!file.is_open()) {
        LOG(WARNING) << "Failed to open config file for writing: " << path;
        return;
    }
    
    file << "# NullWare Browser Configuration\n";
    file << "# Generated automatically - edit with caution\n\n";
    
    file << "# Ad-Blocker Settings\n";
    file << "enable_adblocker=" << (config_.enable_adblocker ? "true" : "false") << "\n\n";
    
    file << "# Security Settings\n";
    file << "https_only_mode=" << (config_.https_only_mode ? "true" : "false") << "\n";
    file << "certificate_pinning=" << (config_.certificate_pinning ? "true" : "false") << "\n";
    file << "csp_enforcement=" << (config_.csp_enforcement ? "true" : "false") << "\n";
    file << "safe_browsing=" << (config_.safe_browsing ? "true" : "false") << "\n";
    file << "sandbox_enabled=" << (config_.sandbox_enabled ? "true" : "false") << "\n\n";
    
    file << "# Developer Settings\n";
    file << "enable_devtools=" << (config_.enable_devtools ? "true" : "false") << "\n";
    file << "remote_debugging=" << (config_.remote_debugging ? "true" : "false") << "\n";
    file << "devtools_port=" << config_.devtools_port << "\n\n";
    
    file << "# Path Settings\n";
    file << "user_data_dir=" << config_.user_data_dir << "\n";
    file << "cache_dir=" << config_.cache_dir << "\n\n";
    
    file << "# Proxy Settings\n";
    file << "proxy_server=" << config_.proxy_server << "\n";
    file << "use_proxy=" << (config_.use_proxy ? "true" : "false") << "\n\n";
    
    file << "# Privacy Settings\n";
    file << "do_not_track=" << (config_.do_not_track ? "true" : "false") << "\n";
    file << "block_third_party_cookies=" << (config_.block_third_party_cookies ? "true" : "false") << "\n\n";
    
    file << "# UI Settings\n";
    file << "default_homepage=" << config_.default_homepage << "\n";
    
    LOG(INFO) << "Saved configuration to: " << path;
}

void ConfigManager::LoadFromCommandLine(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == SWITCH_DISABLE_ADBLOCKER) {
            config_.enable_adblocker = false;
        } else if (arg == SWITCH_ENABLE_ADBLOCKER) {
            config_.enable_adblocker = true;
        } else if (arg == SWITCH_DISABLE_DEVTOOLS) {
            config_.enable_devtools = false;
        } else if (arg == SWITCH_ENABLE_DEVTOOLS) {
            config_.enable_devtools = true;
        } else if (arg == SWITCH_DISABLE_SANDBOX) {
            config_.sandbox_enabled = false;
        } else if (arg == SWITCH_HTTPS_ONLY) {
            config_.https_only_mode = true;
        } else if (arg == SWITCH_INCOGNITO) {
            // Incognito mode - temporary settings
            config_.clear_cookies_on_exit = true;
            config_.clear_cache_on_exit = true;
        } else if (arg == SWITCH_REMOTE_DEBUGGING) {
            config_.remote_debugging = true;
        } else if (arg == SWITCH_FILTER_LIST && i + 1 < argc) {
            config_.custom_filter_files.push_back(argv[++i]);
        } else if (arg == SWITCH_USER_DATA_DIR && i + 1 < argc) {
            config_.user_data_dir = argv[++i];
        } else if (arg == SWITCH_PROXY_SERVER && i + 1 < argc) {
            config_.proxy_server = argv[++i];
            config_.use_proxy = true;
        }
    }
}

}  // namespace nullware
