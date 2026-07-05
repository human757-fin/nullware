#ifndef NULLWARE_BROWSER_RENDER_HANDLER_H_
#define NULLWARE_BROWSER_RENDER_HANDLER_H_

#include <include/cef_base.h>
#include <include/cef_render_handler.h>

#include <vector>

namespace nullware {

// Render handler for CEF
class RenderHandler : public CefRenderHandler {
public:
    RenderHandler();
    ~RenderHandler() override;
    
    // Get the CEF render handler
    CefRefPtr<CefRenderHandler> GetCefRenderHandler();
    
    // CefRenderHandler methods
    virtual void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
    virtual void OnPaint(CefRefPtr<CefBrowser> browser,
                        PaintElementType type,
                        const RectList& dirty_rects,
                        const void* buffer,
                        int width,
                        int height) override;
    virtual void OnAcceleratedPaint(CefRefPtr<CefBrowser> browser,
                                   PaintElementType type,
                                   const RectList& dirty_rects,
                                   void* share_handle) override;
    virtual bool GetScreenInfo(CefRefPtr<CefBrowser> browser,
                               CefScreenInfo& screen_info) override;
    virtual void OnScreenInfoChanged(CefRefPtr<CefBrowser> browser) override;
    virtual void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) override;
    virtual void OnPopupSize(CefRefPtr<CefBrowser> browser, const CefSize& size) override;
    virtual void OnImeCompositionRangeChanged(
        CefRefPtr<CefBrowser> browser,
        const CefRange& selected_range,
        const RectList& character_bounds) override;
    virtual void OnVirtualKeyboardRequested(
        CefRefPtr<CefBrowser> browser,
        TextInputMode input_mode) override;
    
private:
    IMPLEMENT_REFCOUNTING(RenderHandler);
};

}  // namespace nullware

#endif  // NULLWARE_BROWSER_RENDER_HANDLER_H_
