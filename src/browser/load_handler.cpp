#include "src/browser/load_handler.h"

#include "src/common/logging.h"

namespace nullware {

LoadHandler::LoadHandler() {}

LoadHandler::~LoadHandler() {}

CefRefPtr<CefLoadHandler> LoadHandler::GetCefLoadHandler() {
    return this;
}

void LoadHandler::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                     bool is_loading,
                                     bool can_go_back,
                                     bool can_go_forward) {
    LOG_DEBUG("Loading state changed: loading=" + std::string(is_loading ? "true" : "false") +
               ", can_go_back=" + std::string(can_go_back ? "true" : "false") +
               ", can_go_forward=" + std::string(can_go_forward ? "true" : "false"));
}

void LoadHandler::OnLoadStart(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           TransitionType transition_type) {
    LOG_DEBUG("Load started: " + frame->GetURL().ToString() + 
               ", transition_type=" + std::to_string(transition_type));
}

void LoadHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         int http_status_code) {
    LOG_DEBUG("Load ended: " + frame->GetURL().ToString() + 
               " with status: " + std::to_string(http_status_code));
}

void LoadHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           ErrorCode error_code,
                           const CefString& error_text,
                           const CefString& failed_url) {
    LOG_ERROR("Load error: " + error_text.ToString() + 
               " (" + std::to_string(error_code) + ")");
}

}  // namespace nullware
