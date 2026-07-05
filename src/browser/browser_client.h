#ifndef NULLWARE_BROWSER_BROWSER_CLIENT_H_
#define NULLWARE_BROWSER_BROWSER_CLIENT_H_

#include <include/cef_client.h>

#include <string>
#include <vector>
#include <memory>

namespace nullware {

class BrowserWindow;
class RenderHandler;
class LoadHandler;

// Browser client implementation for CEF
class BrowserClient : public CefClient,
                      public CefLifeSpanHandler,
                      public CefLoadHandler {
public:
    BrowserClient();
    ~BrowserClient() override;
    
    // CefClient methods
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override;
    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override;
    virtual CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request,
        bool is_navigation,
        bool is_download,
        const CefString& request_initiator,
        bool& disable_default_handling) override;
    
    // CefLifeSpanHandler methods
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    virtual bool DoClose(CefRefPtr<CefBrowser> browser) override;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;
    
    // CefLoadHandler methods
    virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                     bool is_loading,
                                     bool can_go_back,
                                     bool can_go_forward) override;
    virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           TransitionType transition_type) override;
    virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         int http_status_code) override;
    virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           ErrorCode error_code,
                           const CefString& error_text,
                           const CefString& failed_url) override;
    
    // Set the browser window
    void SetBrowserWindow(BrowserWindow* window);
    
    // Get the browser window
    BrowserWindow* GetBrowserWindow();
    
    // Get the main browser
    CefRefPtr<CefBrowser> GetMainBrowser();
    
    // Set the main browser
    void SetMainBrowser(CefRefPtr<CefBrowser> browser);
    
private:
    BrowserWindow* browser_window_;
    CefRefPtr<CefBrowser> main_browser_;
    std::unique_ptr<RenderHandler> render_handler_;
    std::unique_ptr<LoadHandler> load_handler_;
    
    IMPLEMENT_REFCOUNTING(BrowserClient);
};

}  // namespace nullware

#endif  // NULLWARE_BROWSER_BROWSER_CLIENT_H_
