#ifndef NULLWARE_BROWSER_LOAD_HANDLER_H_
#define NULLWARE_BROWSER_LOAD_HANDLER_H_

#include <include/cef_base.h>
#include <include/cef_load_handler.h>

#include <string>

namespace nullware {

// Load handler for CEF
class LoadHandler : public CefLoadHandler {
public:
    LoadHandler();
    ~LoadHandler() override;
    
    // Get the CEF load handler
    CefRefPtr<CefLoadHandler> GetCefLoadHandler();
    
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
    
private:
    IMPLEMENT_REFCOUNTING(LoadHandler);
};

}  // namespace nullware

#endif  // NULLWARE_BROWSER_LOAD_HANDLER_H_
