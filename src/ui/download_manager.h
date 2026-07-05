#ifndef NULLWARE_UI_DOWNLOAD_MANAGER_H_
#define NULLWARE_UI_DOWNLOAD_MANAGER_H_

#include <include/cef_base.h>
#include <include/cef_download_handler.h>

#include <string>
#include <vector>
#include <memory>
#include <mutex>

namespace nullware {

class MainWindow;

// Download status
enum class DownloadStatus {
    PENDING,
    IN_PROGRESS,
    COMPLETED,
    CANCELLED,
    FAILED,
    INTERRUPTED
};

// Download information
struct DownloadInfo {
    std::string id;
    std::string url;
    std::string filename;
    std::string save_path;
    int64_t total_bytes;
    int64_t received_bytes;
    int percent_complete;
    DownloadStatus status;
    std::string mime_type;
    std::string start_time;
    std::string end_time;
};

// Download manager class
class DownloadManager : public CefDownloadHandler {
public:
    DownloadManager(MainWindow* main_window);
    ~DownloadManager() override;
    
    // Initialize the download manager
    bool Initialize();
    
    // Start a download
    void StartDownload(const std::string& url, const std::string& filename = "");
    
    // Cancel a download
    void CancelDownload(const std::string& download_id);
    
    // Pause a download
    void PauseDownload(const std::string& download_id);
    
    // Resume a download
    void ResumeDownload(const std::string& download_id);
    
    // Get download list
    std::vector<DownloadInfo> GetDownloadList() const;
    
    // Get download by ID
    DownloadInfo GetDownload(const std::string& download_id) const;
    
    // Clear completed downloads
    void ClearCompletedDownloads();
    
    // Clear all downloads
    void ClearAllDownloads();
    
    // Show downloads dialog
    void ShowDownloadsDialog();
    
    // Get download count
    size_t GetDownloadCount() const;
    
    // Get active download count
    size_t GetActiveDownloadCount() const;
    
    // CefDownloadHandler methods
    virtual void OnBeforeDownload(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefDownloadItem> download_item,
        const CefString& suggested_name,
        Callback callback) override;
    
    virtual void OnDownloadUpdated(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefDownloadItem> download_item,
        Callback callback) override;
    
private:
    MainWindow* main_window_;
    std::vector<DownloadInfo> downloads_;
    std::mutex mutex_;
    
    // Generate unique download ID
    std::string GenerateDownloadId() const;
    
    // Get default download directory
    std::string GetDefaultDownloadDirectory() const;
    
    // Format file size
    std::string FormatFileSize(int64_t bytes) const;
    
    // Format time remaining
    std::string FormatTimeRemaining(int64_t seconds) const;
    
    // Update download progress
    void UpdateDownloadProgress(const std::string& download_id,
                               int64_t received_bytes,
                               int64_t total_bytes);
    
    IMPLEMENT_REFCOUNTING(DownloadManager);
};

}  // namespace nullware

#endif  // NULLWARE_UI_DOWNLOAD_MANAGER_H_
