#include "WinApiInstance.hxx"

// Local headers
#include "WinApiWindow.hxx"

// External APIs
#include <SnesRenderApi/VulkanError.hxx>

// Standard library
#include <utility>

namespace maseya::renderer {
constexpr wchar_t dummy_window_class_name[256 + 1] = L"SnesRenderApi.DummyWindow";

ATOM register_dummy_window_class(HINSTANCE hinstance);

WinApiInstance::WinApiInstance(HINSTANCE hinstance)
        : hinstance_(hinstance),
          dummy_window_class_(register_dummy_window_class(hinstance)) {}

WinApiInstance::WinApiInstance(WinApiInstance&& rhs) noexcept
        : hinstance_(std::exchange(rhs.hinstance_, nullptr)),
          dummy_window_class_(rhs.dummy_window_class_) {}

WinApiInstance::~WinApiInstance() {
    if (hinstance_) {
        UnregisterClassW(reinterpret_cast<LPCWSTR>(dummy_window_class_), hinstance_);
    }
}

WinApiInstance& WinApiInstance::operator=(WinApiInstance&& rhs) noexcept {
    std::swap(hinstance_, rhs.hinstance_);
    std::swap(dummy_window_class_, rhs.dummy_window_class_);
    return *this;
}

WinApiWindow WinApiInstance::create_dummy_window() const {
    HWND hwnd = CreateWindowExW(0, reinterpret_cast<LPCWSTR>(dummy_window_class_),
                                L"Dummy window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                                CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL,
                                hinstance_, NULL);
    if (hwnd == 0) {
        throw VulkanWinApiError();
    }
    return WinApiWindow(hwnd);
}

ATOM register_dummy_window_class(HINSTANCE hinstance) {
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = DefWindowProcW;
    wc.hInstance = hinstance;
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = dummy_window_class_name;

    ATOM result = RegisterClassExW(&wc);
    if (result == 0) {
        throw VulkanWinApiError();
    }

    return result;
}
}  // namespace maseya::renderer
