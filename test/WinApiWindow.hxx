#pragma once

// External APIs
#include <Windows.h>

// Standard library
#include <utility>

namespace maseya::renderer {
class WinApiWindow {
public:
    WinApiWindow(HWND hwnd);
    WinApiWindow(const WinApiWindow&) = delete;
    WinApiWindow(WinApiWindow&& rhs) noexcept;

    ~WinApiWindow();

    WinApiWindow& operator=(const WinApiWindow&) = delete;
    WinApiWindow& operator=(WinApiWindow&& rhs) noexcept;

    HWND hwnd() const { return hwnd_; }
    HINSTANCE hinstance() const;
    WNDPROC wndproc() const;

    void show() const;
    void hide() const;

    virtual void run();

    virtual bool run_iteration();

    virtual void on_size(WPARAM size_mode, SHORT width, SHORT height);
    virtual void on_sizing(WPARAM size_mode, RECT& rect);
    virtual void on_move(const POINTS& location);
    virtual void on_moving(RECT& rect);

private:
    static WinApiWindow& get_window(HWND hwnd);

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                                       LPARAM lParam);

private:
    HWND hwnd_;
    MSG msg_;
    WNDPROC def_wnd_proc_;
};
}  // namespace maseya::renderer