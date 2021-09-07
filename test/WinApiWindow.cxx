#include "WinApiWindow.hxx"

// External APIs
#include <SnesRenderApi/VulkanError.hxx>

// Standard library
#include <utility>

namespace maseya::renderer {
WinApiWindow::WinApiWindow(HWND hwnd) : hwnd_(hwnd), msg_(), def_wnd_proc_(wndproc()) {
    SetWindowLongPtrW(hwnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    SetWindowLongPtrW(hwnd_, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProc));
}

WinApiWindow::WinApiWindow(WinApiWindow&& rhs) noexcept
        : hwnd_(std::exchange(rhs.hwnd_, nullptr)),
          msg_(rhs.msg_),
          def_wnd_proc_(rhs.def_wnd_proc_) {}

WinApiWindow::~WinApiWindow() {
    if (hwnd_) {
        DestroyWindow(hwnd_);
    }
}

WinApiWindow& WinApiWindow::operator=(WinApiWindow&& rhs) noexcept {
    std::swap(hwnd_, rhs.hwnd_);
    msg_ = rhs.msg_;
    def_wnd_proc_ = rhs.def_wnd_proc_;
    return *this;
}


HINSTANCE WinApiWindow::hinstance() const {
    return reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(hwnd_, GWLP_HINSTANCE));
}
WNDPROC WinApiWindow::wndproc() const {
    return reinterpret_cast<WNDPROC>(GetWindowLongPtrW(hwnd_, GWLP_WNDPROC));
}

void WinApiWindow::show() const { ShowWindow(hwnd_, SW_SHOW); }
void WinApiWindow::hide() const { ShowWindow(hwnd_, SW_HIDE); }

void WinApiWindow::run() {
    while (run_iteration()) {
        continue;
    }
}

bool WinApiWindow::run_iteration() {
    if (!GetMessageW(&msg_, NULL, 0, 0)) {
        return false;
    }

    TranslateMessage(&msg_);
    DispatchMessageW(&msg_);
    return true;
}

void WinApiWindow::on_size(WPARAM size_mode, SHORT width, SHORT height) {}

void WinApiWindow::on_sizing(WPARAM size_mode, RECT& rect) {}

void WinApiWindow::on_move(const POINTS& location) {}

void WinApiWindow::on_moving(RECT& rect) {}

WinApiWindow& WinApiWindow::get_window(HWND hwnd) {
    return *reinterpret_cast<WinApiWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
}

LRESULT CALLBACK WinApiWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                                          LPARAM lParam) {
    WinApiWindow& window = get_window(hwnd);

    switch (uMsg) {
        case WM_CLOSE:
            PostQuitMessage(static_cast<int>(wParam));
            return 0;

        case WM_SIZE:
            window.on_size(wParam, LOWORD(lParam), HIWORD(lParam));
            break;

        case WM_SIZING:
            window.on_sizing(wParam, *reinterpret_cast<LPRECT>(lParam));
            break;

        case WM_MOVE:
            window.on_move(MAKEPOINTS(lParam));
            break;

        case WM_MOVING:
            window.on_moving(*reinterpret_cast<LPRECT>(lParam));
            break;
    }

    return CallWindowProcW(window.def_wnd_proc_, hwnd, uMsg, wParam, lParam);
}
}  // namespace maseya::renderer