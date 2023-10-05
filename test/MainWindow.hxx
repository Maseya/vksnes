#pragma once

// Local headers
#include "WinApiWindow.hxx"

// External APIs
#include <SnesRenderLib/EasyApi.hxx>

// Standard library
#include <chrono>
#include <vector>

namespace maseya::vksnes {
class MainWindow : public WinApiWindow {
private:
    using time_point = std::chrono::steady_clock::time_point;
    using duration = std::chrono::duration<double, std::ratio<1, 60>>;

public:
    MainWindow(HWND&& hwnd);

    MainWindow(const MainWindow&) = delete;
    MainWindow(MainWindow&&) noexcept = default;

    virtual ~MainWindow();

    MainWindow& operator=(const MainWindow&) = delete;
    MainWindow& operator=(MainWindow&&) noexcept = default;

    void tick();

    void update();

    void draw();

    virtual void run();

    virtual void on_size(WPARAM size_mode, SHORT width, SHORT height);
    virtual void on_move(const POINTS& location);
    virtual void on_mouse_wheel(WORD key_state, WORD z_scroll, WORD x_pos, WORD y_pos);
    virtual void on_key_down(WPARAM key);

    std::vector<uint16_t> palette_data_;
    std::vector<uint8_t> gfx_data_;
    std::vector<uint16_t> obj_data_;
    std::vector<uint8_t> tilemap_data_;

private:
    VkExtent2D extent() const {
        return VkExtent2D{static_cast<uint32_t>(width_),
                          static_cast<uint32_t>(height_)};
    }

    bool recreate_swapchain();

    double current_frame() const;

    static LRESULT CALLBACK WindowProc(HWND hwnd_, UINT uMsg, WPARAM wParam,
                                       LPARAM lParam);

private:
    WNDPROC def_wnd_proc_;
    maseya::vksnes::win32::EasyApi easy_api_;

    maseya::vksnes::Image overlay_image_;
    maseya::vksnes::Sampler overlay_sampler_;
    maseya::vksnes::RenderPass overlay_render_pass_;
    maseya::vksnes::FlatFramebuffer overlay_framebuffer_;

    maseya::vksnes::Image dashed_line_texture_image;
    maseya::vksnes::ImageView dashed_line_texture_image_view_;
    maseya::vksnes::Sampler dashed_line_texture_image_sampler_;

    maseya::vksnes::OverlayRenderer overlay_renderer_;
    maseya::vksnes::LineRenderer line_renderer_;
    maseya::vksnes::CheckerboardRenderer checkerboard_renderer_;
    maseya::vksnes::GradientRenderer gradient_renderer_;
    maseya::vksnes::PaletteRenderer palette_renderer_;
    maseya::vksnes::GfxRenderer gfx_renderer_;
    maseya::vksnes::ObjRenderer obj_renderer_;
    maseya::vksnes::Map16Renderer map16_renderer_;
    maseya::vksnes::ChrRenderer chr_renderer_;

    maseya::vksnes::Surface surface_;
    maseya::vksnes::ManagedSwapchain managed_swapchain_;
    std::vector<maseya::vksnes::FlatFramebuffer> framebuffers_;
    maseya::vksnes::RenderPass present_render_pass_;

    time_point start_time_;

    SHORT width_;
    SHORT height_;

    CheckerboardPattern checkerboard_pattern_;
    GradientPattern gradient_pattern_;
};
}  // namespace maseya::vksnes
