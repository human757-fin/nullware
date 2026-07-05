#include "src/browser/browser_client.h"

#include "src/ui/main_window.h"
#include "src/browser/render_handler.h"
#include "src/browser/load_handler.h"
#include "src/adblocker/ad_blocker.h"
#include "src/common/logging.h"

namespace nullware {

BrowserClient::BrowserClient() : main_window_(nullptr) {}

BrowserClient::~BrowserClient() {}

CefRefPtr<CefLifeSpanHandler> BrowserClient::GetLifeSpanHandler() {
    return this;
}

CefRefPtr<CefLoadHandler> BrowserClient::GetLoadHandler() {
    return this;
}

CefRefPtr<CefRenderHandler> BrowserClient::GetRenderHandler() {
    if (!render_handler_) {
        render_handler_ = std::make_unique<RenderHandler>();
    }
    return render_handler_->GetCefRenderHandler();
}

CefRefPtr<CefResourceRequestHandler> BrowserClient::GetResourceRequestHandler(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    bool is_navigation,
    bool is_download,
    const CefString& request_initiator,
    bool& disable_default_handling) {
    
    // Use our ad-blocker as resource request handler
    auto ad_blocker = AdBlockerManager::GetInstance().GetAdBlocker();
    if (ad_blocker && ad_blocker->IsEnabled()) {
        auto interceptor = ad_blocker->GetRequestInterceptor();
        if (interceptor) {
            disable_default_handling = true;
            return interceptor->GetCefResourceRequestHandler();
        }
    }
    
    return nullptr;
}

CefRefPtr<CefDownloadHandler> BrowserClient::GetDownloadHandler() {
    if (main_window_) {
        return main_window_->GetDownloadManager();
    }
    return nullptr;
}

void BrowserClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    LOG_INFO("Browser created");
    
    if (!main_browser_) {
        main_browser_ = browser;
    }
    
    if (main_window_) {
        main_window_->OnBrowserCreated(browser);
    }
}

bool BrowserClient::DoClose(CefRefPtr<CefBrowser> browser) {
    LOG_INFO("Browser close requested");
    
    if (main_window_) {
        main_window_->OnBrowserCloseRequested(browser);
    }
    
    // Allow the browser to close
    return false;
}

void BrowserClient::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
    LOG_INFO("Browser closing");
    
    if (browser == main_browser_) {
        main_browser_ = nullptr;
    }
    
    if (main_window_) {
        main_window_->OnBrowserClosed(browser);
    }
}

void BrowserClient::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                         bool is_loading,
                                         bool can_go_back,
                                         bool can_go_forward) {
    if (main_window_) {
        main_window_->OnLoadingStateChange(is_loading, can_go_back, can_go_forward);
    }
}

void BrowserClient::OnLoadStart(CefRefPtr<CefBrowser> browser,
                               CefRefPtr<CefFrame> frame,
                               TransitionType transition_type) {
    LOG_DEBUG("Page load started: " + frame->GetURL().ToString());
    
    if (main_window_) {
        main_window_->OnLoadStart(frame->GetURL().ToString());
    }
}

void BrowserClient::OnLoadEnd(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             int http_status_code) {
    LOG_DEBUG("Page load ended: " + frame->GetURL().ToString() + 
               " with status: " + std::to_string(http_status_code));
    
    if (main_window_) {
        main_window_->OnLoadEnd(frame->GetURL().ToString(), http_status_code);
    }
}

void BrowserClient::OnLoadError(CefRefPtr<CefBrowser> browser,
                               CefRefPtr<CefFrame> frame,
                               ErrorCode error_code,
                               const CefString& error_text,
                               const CefString& failed_url) {
    LOG_ERROR("Page load error: " + error_text.ToString() + 
               " (" + std::to_string(error_code) + ")");
    
    if (main_window_) {
        main_window_->OnLoadError(failed_url.ToString(), error_code, error_text.ToString());
    }
}

void BrowserClient::OnBeforeDownload(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefDownloadItem> download_item,
    const CefString& suggested_name,
    Callback callback) {
    
    LOG_INFO("Download starting: " + download_item->GetURL().ToString());
    
    if (main_window_ && main_window_->GetDownloadManager()) {
        // Let the download manager handle it
        main_window_->GetDownloadManager()->OnBeforeDownload(
            browser, download_item, suggested_name, callback
        );
    } else {
        // Default behavior
        callback.Continue(suggested_name.ToString(), false);
    }
}

void BrowserClient::OnDownloadUpdated(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefDownloadItem> download_item,
    Callback callback) {
    
    if (main_window_ && main_window_->GetDownloadManager()) {
        main_window_->GetDownloadManager()->OnDownloadUpdated(
            browser, download_item, callback
        );
    }
}

void BrowserClient::SetMainWindow(MainWindow* window) {
    main_window_ = window;
}

MainWindow* BrowserClient::GetMainWindow() {
    return main_window_;
}

CefRefPtr<CefBrowser> BrowserClient::GetMainBrowser() {
    return main_browser_;
}

void BrowserClient::SetMainBrowser(CefRefPtr<CefBrowser> browser) {
    main_browser_ = browser;
}

}  // namespace nullware
