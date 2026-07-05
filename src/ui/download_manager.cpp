#include "src/ui/download_manager.h"

#include "src/ui/main_window.h"
#include "src/common/logging.h"
#include "src/common/string_utils.h"
#include "src/common/config.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace nullware {

DownloadManager::DownloadManager(MainWindow* main_window)
    : main_window_(main_window) {}

DownloadManager::~DownloadManager() {
    // Clean up any active downloads
    ClearAllDownloads();
}

bool DownloadManager::Initialize() {
    LOG_INFO("Initializing DownloadManager");
    return true;
}

void DownloadManager::StartDownload(const std::string& url, const std::string& filename) {
    LOG_INFO("Starting download: " + url);
    
    // In a real implementation, this would start the download through CEF
    // For now, we'll simulate it
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    DownloadInfo download;
    download.id = GenerateDownloadId();
    download.url = url;
    download.filename = filename.empty() ? ExtractDomain(url) : filename;
    download.save_path = GetDefaultDownloadDirectory() + PATH_SEPARATOR + download.filename;
    download.total_bytes = 0;
    download.received_bytes = 0;
    download.percent_complete = 0;
    download.status = DownloadStatus::PENDING;
    download.mime_type = "application/octet-stream";
    download.start_time = GetCurrentTimestamp();
    download.end_time = "";
    
    downloads_.push_back(download);
    
    LOG_INFO("Download started: " + download.id + " - " + url);
}

void DownloadManager::CancelDownload(const std::string& download_id) {
    LOG_INFO("Cancelling download: " + download_id);
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& download : downloads_) {
        if (download.id == download_id && 
            (download.status == DownloadStatus::PENDING || 
             download.status == DownloadStatus::IN_PROGRESS)) {
            download.status = DownloadStatus::CANCELLED;
            download.end_time = GetCurrentTimestamp();
            LOG_INFO("Download cancelled: " + download_id);
            break;
        }
    }
}

void DownloadManager::PauseDownload(const std::string& download_id) {
    LOG_INFO("Pausing download: " + download_id);
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& download : downloads_) {
        if (download.id == download_id && 
            download.status == DownloadStatus::IN_PROGRESS) {
            download.status = DownloadStatus::INTERRUPTED;
            LOG_INFO("Download paused: " + download_id);
            break;
        }
    }
}

void DownloadManager::ResumeDownload(const std::string& download_id) {
    LOG_INFO("Resuming download: " + download_id);
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& download : downloads_) {
        if (download.id == download_id && 
            download.status == DownloadStatus::INTERRUPTED) {
            download.status = DownloadStatus::IN_PROGRESS;
            LOG_INFO("Download resumed: " + download_id);
            break;
        }
    }
}

std::vector<DownloadInfo> DownloadManager::GetDownloadList() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return downloads_;
}

DownloadInfo DownloadManager::GetDownload(const std::string& download_id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& download : downloads_) {
        if (download.id == download_id) {
            return download;
        }
    }
    
    return DownloadInfo();
}

void DownloadManager::ClearCompletedDownloads() {
    LOG_INFO("Clearing completed downloads");
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = downloads_.begin();
    while (it != downloads_.end()) {
        if (it->status == DownloadStatus::COMPLETED || 
            it->status == DownloadStatus::CANCELLED ||
            it->status == DownloadStatus::FAILED) {
            it = downloads_.erase(it);
        } else {
            ++it;
        }
    }
}

void DownloadManager::ClearAllDownloads() {
    LOG_INFO("Clearing all downloads");
    
    std::lock_guard<std::mutex> lock(mutex_);
    downloads_.clear();
}

void DownloadManager::ShowDownloadsDialog() {
    LOG_INFO("Showing downloads dialog");
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    LOG_INFO("=== Downloads ===");
    for (const auto& download : downloads_) {
        std::string status_str;
        switch (download.status) {
            case DownloadStatus::PENDING: status_str = "Pending"; break;
            case DownloadStatus::IN_PROGRESS: status_str = "In Progress"; break;
            case DownloadStatus::COMPLETED: status_str = "Completed"; break;
            case DownloadStatus::CANCELLED: status_str = "Cancelled"; break;
            case DownloadStatus::FAILED: status_str = "Failed"; break;
            case DownloadStatus::INTERRUPTED: status_str = "Paused"; break;
            default: status_str = "Unknown"; break;
        }
        
        LOG_INFO("ID: " + download.id + 
                 " | Status: " + status_str + 
                 " | Progress: " + std::to_string(download.percent_complete) + "%" + 
                 " | File: " + download.filename + 
                 " | Size: " + FormatFileSize(download.total_bytes));
    }
    LOG_INFO("==================");
}

size_t DownloadManager::GetDownloadCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return downloads_.size();
}

size_t DownloadManager::GetActiveDownloadCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    size_t count = 0;
    for (const auto& download : downloads_) {
        if (download.status == DownloadStatus::PENDING || 
            download.status == DownloadStatus::IN_PROGRESS) {
            count++;
        }
    }
    return count;
}

void DownloadManager::OnBeforeDownload(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefDownloadItem> download_item,
    const CefString& suggested_name,
    Callback callback) {
    
    LOG_INFO("OnBeforeDownload called for: " + download_item->GetURL().ToString());
    
    // Get download information
    std::string url = download_item->GetURL().ToString();
    std::string filename = suggested_name.ToString();
    int64_t total_bytes = download_item->GetTotalBytes();
    std::string mime_type = download_item->GetMimeType().ToString();
    
    // Create download info
    std::lock_guard<std::mutex> lock(mutex_);
    
    DownloadInfo download;
    download.id = GenerateDownloadId();
    download.url = url;
    download.filename = filename.empty() ? ExtractDomain(url) : filename;
    download.save_path = GetDefaultDownloadDirectory() + PATH_SEPARATOR + download.filename;
    download.total_bytes = total_bytes;
    download.received_bytes = 0;
    download.percent_complete = 0;
    download.status = DownloadStatus::IN_PROGRESS;
    download.mime_type = mime_type;
    download.start_time = GetCurrentTimestamp();
    download.end_time = "";
    
    downloads_.push_back(download);
    
    LOG_INFO("Download started via CEF: " + download.id + " - " + url);
    
    // Continue with the download
    callback.Continue(download.save_path, false);
}

void DownloadManager::OnDownloadUpdated(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefDownloadItem> download_item,
    Callback callback) {
    
    std::string url = download_item->GetURL().ToString();
    int64_t received_bytes = download_item->GetReceivedBytes();
    int64_t total_bytes = download_item->GetTotalBytes();
    bool is_complete = download_item->IsComplete();
    bool is_cancelled = download_item->IsCancelled();
    bool is_interrupted = download_item->IsInterrupted();
    
    LOG_DEBUG("Download update: " + url + 
               " - Received: " + std::to_string(received_bytes) + 
               "/" + std::to_string(total_bytes));
    
    // Find the download
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& download : downloads_) {
        if (download.url == url) {
            download.received_bytes = received_bytes;
            download.total_bytes = total_bytes;
            
            if (total_bytes > 0) {
                download.percent_complete = static_cast<int>(
                    (static_cast<double>(received_bytes) / total_bytes) * 100.0
                );
            }
            
            if (is_complete) {
                download.status = DownloadStatus::COMPLETED;
                download.end_time = GetCurrentTimestamp();
                LOG_INFO("Download completed: " + download.id);
            } else if (is_cancelled) {
                download.status = DownloadStatus::CANCELLED;
                download.end_time = GetCurrentTimestamp();
                LOG_INFO("Download cancelled: " + download.id);
            } else if (is_interrupted) {
                download.status = DownloadStatus::INTERRUPTED;
                LOG_INFO("Download interrupted: " + download.id);
            }
            
            break;
        }
    }
}

std::string DownloadManager::GenerateDownloadId() const {
    // Generate a unique ID using timestamp and random number
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto value = now_ms.time_since_epoch().count();
    
    return "dl_" + std::to_string(value) + "_" + GenerateRandomString(4);
}

std::string DownloadManager::GetDefaultDownloadDirectory() const {
    // Get from config or use default
    const auto& config = ConfigManager::GetInstance().GetConfig();
    
    if (!config.cache_dir.empty()) {
        return config.cache_dir + PATH_SEPARATOR + "downloads";
    }
    
    return "downloads";
}

std::string DownloadManager::FormatFileSize(int64_t bytes) const {
    if (bytes < 1024) {
        return std::to_string(bytes) + " B";
    } else if (bytes < 1024 * 1024) {
        return std::to_string(bytes / 1024) + " KB";
    } else if (bytes < 1024 * 1024 * 1024) {
        return std::to_string(bytes / (1024 * 1024)) + " MB";
    } else {
        return std::to_string(bytes / (1024 * 1024 * 1024)) + " GB";
    }
}

std::string DownloadManager::FormatTimeRemaining(int64_t seconds) const {
    if (seconds < 60) {
        return std::to_string(seconds) + "s";
    } else if (seconds < 3600) {
        int minutes = static_cast<int>(seconds / 60);
        return std::to_string(minutes) + "m";
    } else {
        int hours = static_cast<int>(seconds / 3600);
        int minutes = static_cast<int>((seconds % 3600) / 60);
        return std::to_string(hours) + "h " + std::to_string(minutes) + "m";
    }
}

void DownloadManager::UpdateDownloadProgress(const std::string& download_id,
                                           int64_t received_bytes,
                                           int64_t total_bytes) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& download : downloads_) {
        if (download.id == download_id) {
            download.received_bytes = received_bytes;
            download.total_bytes = total_bytes;
            
            if (total_bytes > 0) {
                download.percent_complete = static_cast<int>(
                    (static_cast<double>(received_bytes) / total_bytes) * 100.0
                );
            }
            break;
        }
    }
}

}  // namespace nullware
