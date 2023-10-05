#include "WinApiInstance.hxx"

// External APIs
#include <vkbase/VulkanError.hxx>

namespace maseya::vksnes {
constexpr wchar_t dummy_window_class_name[256 + 1] = L"SnesRenderLib.DummyWindow";

static ATOM register_dummy_window_class(HINSTANCE hinstance);

WinApiInstance::WindowClassRemover::WindowClassRemover(HINSTANCE hinstance) noexcept
        : hinstance_(hinstance) {}

void WinApiInstance::WindowClassRemover::operator()(ATOM atom) const noexcept {
    UnregisterClassW(reinterpret_cast<LPCWSTR>(atom), hinstance_);
}

WinApiInstance::WinApiInstance(HINSTANCE hinstance)
        : hinstance_(hinstance),
          dummy_window_class_(register_dummy_window_class(hinstance), hinstance) {}

HWND WinApiInstance::create_dummy_hwnd(int client_width, int client_height) const {
    int width, height;
    get_window_size_from_client(client_width, client_height, width, height);

    HWND hwnd = CreateWindowExW(0, reinterpret_cast<LPCWSTR>(*dummy_window_class_),
                                L"Dummy window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                                CW_USEDEFAULT, width, height, nullptr, nullptr,
                                hinstance_, nullptr);
    if (hwnd == 0) {
        throw vkbase::VulkanWinApiError();
    }
    return hwnd;
}

void WinApiInstance::get_window_size_from_client(int client_width, int client_height,
                                                 int& window_width,
                                                 int& window_height) {
    int cxSizeFrame = GetSystemMetrics(SM_CXSIZEFRAME);
    int cxPadding = GetSystemMetrics(SM_CXPADDEDBORDER);

    int cySizeFrame = GetSystemMetrics(SM_CYSIZEFRAME);
    int cyCaption = GetSystemMetrics(SM_CYCAPTION);

    window_width = client_width + 2 * (cxSizeFrame + cxPadding);
    window_height = client_height + cyCaption + 2 * (cySizeFrame + cxPadding);
}

ATOM register_dummy_window_class(HINSTANCE hinstance) {
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = DefWindowProcW;
    wc.hInstance = hinstance;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = dummy_window_class_name;

    ATOM result = RegisterClassExW(&wc);
    if (result == 0) {
        throw vkbase::VulkanWinApiError();
    }
    return result;
}
}  // namespace maseya::vksnes
