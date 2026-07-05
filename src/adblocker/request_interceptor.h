#ifndef NULLWARE_ADBLOCKER_REQUEST_INTERCEPTOR_H_
#define NULLWARE_ADBLOCKER_REQUEST_INTERCEPTOR_H_

#include <include/cef_base.h>
#include <include/cef_resource_request_handler.h>

#include <string>
#include <memory>

namespace nullware {

class AdBlocker;

// Request interceptor for CEF integration
class RequestInterceptor : public CefResourceRequestHandler {
public:
    explicit RequestInterceptor(AdBlocker* ad_blocker);
    ~RequestInterceptor() override;
    
    // Get the CEF resource request handler
    CefRefPtr<CefResourceRequestHandler> GetCefResourceRequestHandler();
    
    // CefResourceRequestHandler methods
    virtual ReturnValue OnBeforeResourceLoad(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request,
        Callback callback) override;
    
    virtual CefRefPtr<CefResourceHandler> GetResourceHandler(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request) override;
    
    virtual void OnResourceRedirect(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request,
        CefRefPtr<CefResponse> response,
        CefRefPtr<CefRequest> new_request) override;
    
    virtual bool OnResourceResponse(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request,
        CefRefPtr<CefResponse> response) override;
    
    virtual void OnResourceLoadComplete(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request,
        CefRefPtr<CefResponse> response,
        URLRequestStatus status,
        int64_t received_content_length) override;
    
    virtual void OnProtocolExecution(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request,
        Callback callback) override;
    
    // Set the ad blocker
    void SetAdBlocker(AdBlocker* ad_blocker);
    
private:
    AdBlocker* ad_blocker_;
    
    IMPLEMENT_REFCOUNTING(RequestInterceptor);
};

}  // namespace nullware

#endif  // NULLWARE_ADBLOCKER_REQUEST_INTERCEPTOR_H_
