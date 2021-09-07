#include "MainWindow.hxx"

// External APIs
#include <SnesRenderApi/create_buffer.hxx>

// Standard library
#include <iostream>

namespace maseya::renderer {
constexpr bool verbose_output = false;

vk::Bool32 print_debug_message(
        vk::DebugUtilsMessageSeverityFlagBitsEXT message_severity,
        vk::DebugUtilsMessageTypeFlagsEXT message_type,
        const vk::DebugUtilsMessengerCallbackDataEXT& callback_data);

MainWindow::MainWindow(WinApiInstance&& win_api_instance)
        : WinApiWindow(win_api_instance.create_dummy_window()),
          def_wnd_proc_(wndproc()),
          win_api_instance_(std::move(win_api_instance)),
          instance_("Test app", print_debug_message),
          surface_(instance_, hwnd()),
          uniform_buffers_(surface_.create_test_surface_buffers()),
          vertex_buffers_(surface_.create_vertex_buffer_helper()),
          frame_() {
    SetWindowTextW(hwnd(), L"Render Test window");
    SetTimer(hwnd(), 0, 1, NULL);
    SetWindowLongPtrW(hwnd(), GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProc));

    vertex_buffers_.update_buffers(vertices, std::size(vertices), UINT64_MAX);
}

MainWindow::~MainWindow() { KillTimer(hwnd(), 0); }

void MainWindow::tick() {
    draw();

    if (frame_ == 100) {
        vertex_buffers_.update_buffers(vertices2, std::size(vertices2), UINT64_MAX);
    }
}

void MainWindow::draw() {
loop_:
    RenderTarget target = surface_.acquire_render_target();
    if (!target) {
        return;
    }

    instance_.render_test_image_with_buffers(target, uniform_buffers_, vertex_buffers_,
                                             frame_ * 0.01f);
    if (!surface_.present(target)) {
        goto loop_;
    }
    frame_++;
}

void MainWindow::run() {
    show();
    WinApiWindow::run();
}

void MainWindow::on_size(WPARAM size_mode, SHORT width, SHORT height) { draw(); }

void MainWindow::on_move(const POINTS& location) { draw(); }

MainWindow& MainWindow::get_window(HWND hwnd) {
    return *reinterpret_cast<MainWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
}

LRESULT CALLBACK MainWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                                        LPARAM lParam) {
    MainWindow& window = get_window(hwnd);
    switch (uMsg) {
        case WM_TIMER:
            switch (wParam) {
                case 0:
                    window.tick();
                    break;

                default:
                    break;
            }
            break;
    }

    return CallWindowProcW(window.def_wnd_proc_, hwnd, uMsg, wParam, lParam);
}

vk::Bool32 print_debug_message(
        vk::DebugUtilsMessageSeverityFlagBitsEXT message_severity,
        vk::DebugUtilsMessageTypeFlagsEXT message_type,
        const vk::DebugUtilsMessengerCallbackDataEXT& callback_data) {
    // Ignore verbose messages. Right now, we only care about warnings and errors.
    if (message_severity > vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose) {
        std::cerr << callback_data.pMessage << std::endl << std::endl;
    } else if (verbose_output) {
        std::cout << callback_data.pMessage << std::endl << std::endl;
    }
    return false;
}
}  // namespace maseya::renderer