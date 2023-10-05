#pragma once

// External APIs
#include <Windows.h>

namespace maseya::vksnes {
class WinApiWindow {
public:
    WinApiWindow(HWND&& hwnd);
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

protected:
    virtual void on_size(WPARAM size_mode, SHORT width, SHORT height);
    virtual void on_sizing(WPARAM size_mode, RECT& rect);
    virtual void on_move(const POINTS& location);
    virtual void on_moving(RECT& rect);

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                                       LPARAM lParam);

private:
    HWND hwnd_;
    WNDPROC def_wnd_proc_;
};
}  // namespace maseya::vksnes