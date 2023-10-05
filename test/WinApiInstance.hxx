#pragma once

// External APIs
#include <Windows.h>

#include <vkbase/UniqueObject.hxx>

// Local headers
#include <string>

namespace maseya::vksnes {
class WinApiInstance {
    struct WindowClassRemover {
        constexpr WindowClassRemover() noexcept : hinstance_(nullptr) {}

        WindowClassRemover(HINSTANCE hinstance) noexcept;

        void operator()(ATOM atom) const noexcept;

        HINSTANCE hinstance_;
    };

public:
    WinApiInstance() : WinApiInstance(GetModuleHandleW(nullptr)) {}

    WinApiInstance(HINSTANCE hinstance);

    WinApiInstance(const WinApiInstance&) = delete;
    WinApiInstance(WinApiInstance&&) noexcept = default;

    WinApiInstance& operator=(const WinApiInstance&) = delete;
    WinApiInstance& operator=(WinApiInstance&&) noexcept = default;

    HINSTANCE hinstance() const noexcept { return hinstance_; }

    HWND create_dummy_hwnd(int client_width, int client_height) const;

    static void get_window_size_from_client(int client_width, int client_height,
                                            int& window_width, int& window_height);

private:
    HINSTANCE hinstance_;
    vkbase::UniqueObject<ATOM, WindowClassRemover> dummy_window_class_;
};
}  // namespace maseya::vksnes