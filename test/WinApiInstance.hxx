#pragma once

// External APIs
#include <Windows.h>

// Local headers
#include <string>

namespace maseya::renderer {
class WinApiInstance;

typedef WinApiInstance* WinApiHandle;

class WinApiWindow;

class WinApiInstance {
public:
    WinApiInstance() : WinApiInstance(GetModuleHandleW(nullptr)) {}

    WinApiInstance(HINSTANCE hinstance);

    WinApiInstance(const WinApiInstance&) = delete;
    WinApiInstance(WinApiInstance&&) noexcept;

    ~WinApiInstance();

    WinApiInstance& operator=(const WinApiInstance&) = delete;
    WinApiInstance& operator=(WinApiInstance&&) noexcept;

    HINSTANCE hinstance() const { return hinstance_; }

    WinApiWindow create_dummy_window() const;

private:
    HINSTANCE hinstance_;
    ATOM dummy_window_class_;
};
}  // namespace maseya::renderer