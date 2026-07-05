#include "src/browser/browser_client.h"

#include "src/browser/browser_window.h"
#include "src/browser/render_handler.h"
#include "src/browser/load_handler.h"
#include "src/adblocker/ad_blocker.h"
#include "src/common/logging.h"

namespace nullware {

BrowserClient::BrowserClient() : browser_window_(nullptr) {}

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

void BrowserClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    LOG_INFO("Browser created");
    
    if (!main_browser_) {
        main_browser_ = browser;
    }
    
    if (browser_window_) {
        browser_window_->OnBrowserCreated(browser);
    }
}

bool BrowserClient::DoClose(CefRefPtr<CefBrowser> browser) {
    LOG_INFO("Browser close requested");
    
    if (browser_window_) {
        browser_window_->OnBrowserCloseRequested(browser);
    }
    
    // Allow the browser to close
    return false;
}

void BrowserClient::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
    LOG_INFO("Browser closing");
    
    if (browser == main_browser_) {
        main_browser_ = nullptr;
    }
    
    if (browser_window_) {
        browser_window_->OnBrowserClosed(browser);
    }
}

void BrowserClient::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                         bool is_loading,
                                         bool can_go_back,
                                         bool can_go_forward) {
    if (browser_window_) {
        browser_window_->OnLoadingStateChange(browser, is_loading, can_go_back, can_go_forward);
    }
}

void BrowserClient::OnLoadStart(CefRefPtr<CefBrowser> browser,
                               CefRefPtr<CefFrame> frame,
                               TransitionType transition_type) {
    LOG_DEBUG("Page load started: " + frame->GetURL().ToString());
    
    if (browser_window_) {
        browser_window_->OnLoadStart(browser, frame, transition_type);
    }
}

void BrowserClient::OnLoadEnd(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             int http_status_code) {
    LOG_DEBUG("Page load ended: " + frame->GetURL().ToString() + 
               " with status: " + std::to_string(http_status_code));
    
    if (browser_window_) {
        browser_window_->OnLoadEnd(browser, frame, http_status_code);
    }
}

void BrowserClient::OnLoadError(CefRefPtr<CefBrowser> browser,
                               CefRefPtr<CefFrame> frame,
                               ErrorCode error_code,
                               const CefString& error_text,
                               const CefString& failed_url) {
    LOG_ERROR("Page load error: " + error_text.ToString() + 
               " (" + std::to_string(error_code) + ")");
    
    if (browser_window_) {
        browser_window_->OnLoadError(browser, frame, error_code, error_text, failed_url);
    }
}

void BrowserClient::SetBrowserWindow(BrowserWindow* window) {
    browser_window_ = window;
}

BrowserWindow* BrowserClient::GetBrowserWindow() {
    return browser_window_;
}

CefRefPtr<CefBrowser> BrowserClient::GetMainBrowser() {
    return main_browser_;
}

void BrowserClient::SetMainBrowser(CefRefPtr<CefBrowser> browser) {
    main_browser_ = browser;
}

}  // namespace nullware
