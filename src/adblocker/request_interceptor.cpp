#include "src/adblocker/request_interceptor.h"

#include "src/adblocker/ad_blocker.h"
#include "src/common/logging.h"
#include "src/common/string_utils.h"

namespace nullware {

RequestInterceptor::RequestInterceptor(AdBlocker* ad_blocker)
    : ad_blocker_(ad_blocker) {}

RequestInterceptor::~RequestInterceptor() {}

CefRefPtr<CefResourceRequestHandler> RequestInterceptor::GetCefResourceRequestHandler() {
    return this;
}

CefResourceRequestHandler::ReturnValue RequestInterceptor::OnBeforeResourceLoad(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    Callback callback) {
    
    if (!ad_blocker_ || !ad_blocker_->IsEnabled()) {
        return RV_CONTINUE;
    }
    
    // Get URL and source URL
    std::string url = request->GetURL().ToString();
    std::string source_url = "";
    if (frame) {
        source_url = frame->GetURL().ToString();
    }
    
    // Check if URL should be blocked
    if (ad_blocker_->ShouldBlockUrl(url, source_url)) {
        LOG_DEBUG("Request blocked by ad-blocker: " + url);
        return RV_CANCEL;
    }
    
    // Check if request should be blocked (more detailed check)
    std::vector<std::pair<std::string, std::string>> headers;
    CefRequest::HeaderMap header_map;
    request->GetHeaderMap(header_map);
    
    for (const auto& header : header_map) {
        headers.emplace_back(header.first.ToString(), header.second.ToString());
    }
    
    if (ad_blocker_->ShouldBlockRequest(url, request->GetMethod().ToString(), headers, source_url)) {
        LOG_DEBUG("Request blocked by detailed check: " + url);
        return RV_CANCEL;
    }
    
    return RV_CONTINUE;
}

CefRefPtr<CefResourceHandler> RequestInterceptor::GetResourceHandler(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request) {
    // Return nullptr to use default resource handler
    return nullptr;
}

void RequestInterceptor::OnResourceRedirect(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    CefRefPtr<CefResponse> response,
    CefRefPtr<CefRequest> new_request) {
    
    // Check the new URL as well
    if (!ad_blocker_ || !ad_blocker_->IsEnabled()) {
        return;
    }
    
    std::string new_url = new_request->GetURL().ToString();
    std::string source_url = frame ? frame->GetURL().ToString() : "";
    
    if (ad_blocker_->ShouldBlockUrl(new_url, source_url)) {
        LOG_DEBUG("Redirect blocked by ad-blocker: " + new_url);
        // Cancel the redirect by not continuing
        new_request->Cancel();
    }
}

bool RequestInterceptor::OnResourceResponse(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    CefRefPtr<CefResponse> response) {
    
    // Check response headers for ad-related content
    if (!ad_blocker_ || !ad_blocker_->IsEnabled()) {
        return false;
    }
    
    // Check if this is an ad response based on headers
    CefResponse::HeaderMap header_map;
    response->GetHeaderMap(header_map);
    
    for (const auto& header : header_map) {
        std::string header_name = ToLower(header.first.ToString());
        std::string header_value = ToLower(header.second.ToString());
        
        // Check for common ad-related headers
        if (Contains(header_name, "ad") || 
            Contains(header_name, "banner") || 
            Contains(header_name, "tracking") ||
            Contains(header_value, "ad") ||
            Contains(header_value, "banner") ||
            Contains(header_value, "tracking")) {
            LOG_DEBUG("Response blocked due to ad-related headers: " + request->GetURL().ToString());
            return true;  // Cancel the response
        }
    }
    
    return false;  // Continue with the response
}

void RequestInterceptor::OnResourceLoadComplete(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    CefRefPtr<CefResponse> response,
    URLRequestStatus status,
    int64_t received_content_length) {
    
    // Resource load completed
    LOG_DEBUG("Resource load completed: " + request->GetURL().ToString() +
               " status: " + std::to_string(status) +
               " length: " + std::to_string(received_content_length));
}

void RequestInterceptor::OnProtocolExecution(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    Callback callback) {
    
    // Handle protocol execution (e.g., javascript:, data:, etc.)
    std::string url = request->GetURL().ToString();
    
    if (StartsWith(ToLower(url), "javascript:") || 
        StartsWith(ToLower(url), "data:") ||
        StartsWith(ToLower(url), "about:")) {
        // Allow these protocols
        callback.Continue();
        return;
    }
    
    // For other protocols, check with ad-blocker
    if (!ad_blocker_ || !ad_blocker_->IsEnabled()) {
        callback.Continue();
        return;
    }
    
    std::string source_url = frame ? frame->GetURL().ToString() : "";
    if (ad_blocker_->ShouldBlockUrl(url, source_url)) {
        LOG_DEBUG("Protocol execution blocked: " + url);
        callback.Cancel();
    } else {
        callback.Continue();
    }
}

void RequestInterceptor::SetAdBlocker(AdBlocker* ad_blocker) {
    ad_blocker_ = ad_blocker;
}

}  // namespace nullware
