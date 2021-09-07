#include "get_default_format.hxx"

// Local headers
#include "SwapChainSupportDetails.hxx"
#include "VulkanError.hxx"

// Standard library
#include <utility>

namespace maseya::renderer {
namespace {
class DummyWindow {
public:
    DummyWindow();

    ~DummyWindow();

    vk::Win32SurfaceCreateInfoKHR surface_info() const;

private:
    HWND hwnd_;
};
}  // namespace

vk::Format get_default_format(vk::Instance instance,
                              vk::PhysicalDevice physical_device) {
    DummyWindow window;
    vk::UniqueSurfaceKHR surface =
            instance.createWin32SurfaceKHRUnique(window.surface_info());
    SwapChainSupportDetails details(physical_device, *surface);
    if (!details) {
        return vk::Format::eUndefined;
    }
    return details.format();
}

namespace {
HWND create_dummy_hwnd();

DummyWindow::DummyWindow() : hwnd_(create_dummy_hwnd()) {}

HWND create_dummy_hwnd() {
    HWND result = CreateWindowExW(0, L"STATIC", L"dummy", WS_VISIBLE, 0, 0, 100, 100,
                                  NULL, NULL, NULL, NULL);
    if (!result) {
        throw VulkanWinApiError();
    }
    return result;
}

DummyWindow::~DummyWindow() {
    if (hwnd_) {
        DestroyWindow(hwnd_);
    }
}

vk::Win32SurfaceCreateInfoKHR DummyWindow::surface_info() const {
    vk::Win32SurfaceCreateInfoKHR result;
    result.hinstance =
            reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(hwnd_, GWLP_HINSTANCE));
    result.hwnd = hwnd_;
    return result;
}
}  // namespace
}  // namespace maseya::renderer