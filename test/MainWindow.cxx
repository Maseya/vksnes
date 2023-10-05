#include "MainWindow.hxx"

// Standard library
#include <chrono>
#include <iostream>
#include <sstream>

// External APIs
#include <SnesRenderLib/stb_image_write.h>
#include <windowsx.h>

#include <SnesRenderLib/PixelFormat.hxx>
#include <SnesRenderLib/StbImage.hxx>
#include <glm/trigonometric.hpp>

// Local headers
#include "print_debug_message.hxx"

namespace maseya::vksnes {
constexpr PixelFormat formats[] = {
        PixelFormat::Format1Bpp8x8,  PixelFormat::Format2BppNes,
        PixelFormat::Format2BppGb,   PixelFormat::Format2BppNgp,
        PixelFormat::Format2BppVb,   PixelFormat::Format3BppSnes,
        PixelFormat::Format3Bpp8x8,  PixelFormat::Format4BppSnes,
        PixelFormat::Format4BppGba,  PixelFormat::Format4BppSms,
        PixelFormat::Format4BppMsx2, PixelFormat::Format4Bpp8x8,
        PixelFormat::Format8BppSnes, PixelFormat::Format8BppMode7,
};

static MainWindow& get_window(HWND hwnd);

MainWindow::MainWindow(HWND&& hwnd)
        : WinApiWindow(std::move(hwnd)),
          def_wnd_proc_(wndproc()),
          easy_api_("Test app", print_debug_message),
          overlay_image_(easy_api_.create_image(VkExtent2D{0x100, 0x100})),
          overlay_sampler_(easy_api_.device()),
          overlay_render_pass_(easy_api_.create_flat_render_pass(
                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)),
          overlay_framebuffer_(easy_api_.create_flat_framebuffer(overlay_image_)),
          dashed_line_texture_image(nullptr),
          dashed_line_texture_image_view_(nullptr),
          dashed_line_texture_image_sampler_(easy_api_.device(),
                                             VK_SAMPLER_ADDRESS_MODE_REPEAT),
          overlay_renderer_(easy_api_.create_overlay_renderer()),
          line_renderer_(easy_api_.create_line_renderer()),
          checkerboard_renderer_(easy_api_.create_checkerboard_renderer()),
          gradient_renderer_(easy_api_.create_gradient_renderer()),
          palette_renderer_(easy_api_.create_palette_renderer()),
          gfx_renderer_(easy_api_.create_gfx_renderer()),
          obj_renderer_(easy_api_.create_obj_renderer()),
          map16_renderer_(easy_api_.create_map16_renderer()),
          chr_renderer_(easy_api_.create_chr_renderer()),
          surface_(easy_api_.create_surface(this->hwnd())),
          managed_swapchain_(easy_api_.create_managed_swapchain(*surface_)),
          framebuffers_(),
          present_render_pass_(
                  easy_api_.create_flat_render_pass(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)),
          width_{},
          height_{} {
    glm::vec4 colors[2] = {glm::vec4(1, 1, 1, 1), glm::vec4(0, 0, 0, 1)};
    uint32_t sizes[2] = {1, 1};
    dashed_line_texture_image = easy_api_.create_dashed_line(colors, sizes);
    dashed_line_texture_image_view_ =
            easy_api_.create_image_view(dashed_line_texture_image);

    SetWindowTextW(this->hwnd(), L"Render Test window");
    SetTimer(this->hwnd(), 0, 1, NULL);
    SetWindowLongPtrW(this->hwnd(), GWLP_WNDPROC,
                      reinterpret_cast<LONG_PTR>(WindowProc));

    checkerboard_pattern_ = {{{0.7, 0.7, 0.7, 1.0}, {0.5, 0.5, 0.5, 1.0}}, {4, 4}};
    gradient_pattern_ = {
            {{0.6, 0.2, 1.0, 0.5}, {0.2, 1.0, 0.5, 0.5}}, glm::radians(-45.0f), 0};
    gfx_renderer_.format() = maseya::vksnes::PixelFormat::Format8BppSnes;

    obj_renderer_.map16() = true;
    obj_renderer_.format() = maseya::vksnes::PixelFormat::Format4BppSnes;

    map16_renderer_.map16() = true;
    map16_renderer_.format() = maseya::vksnes::PixelFormat::Format4BppSnes;

    chr_renderer_.format() = maseya::vksnes::PixelFormat::Format4BppSnes;

    overlay_renderer_.set_image_view(overlay_framebuffer_.image_view(),
                                     *overlay_sampler_);
    line_renderer_.set_line_texture(*dashed_line_texture_image_view_,
                                    *dashed_line_texture_image_sampler_);

    if (static_cast<bool>(*managed_swapchain_)) {
        framebuffers_ = easy_api_.create_flat_framebuffers(*managed_swapchain_);
    }
}

MainWindow::~MainWindow() {
    KillTimer(hwnd(), 0);
    vkDeviceWaitIdle(easy_api_.device());
}

void MainWindow::tick() {
    if (is_minimized(extent())) {
        return;
    }

    // Update and prepare the draw info.
    update();

    draw();
}

void MainWindow::update() {
    VkExtent2D extent = {static_cast<uint32_t>(width_), static_cast<uint32_t>(height_)};

    checkerboard_renderer_.checkerboard_pattern() = checkerboard_pattern_;

    gradient_renderer_.frame_size() = extent;
    gradient_renderer_.gradient_pattern() = gradient_pattern_;

    palette_renderer_.stride() = 0x10;
    palette_renderer_.set_palette_buffer(palette_data_);

    gfx_renderer_.stride() = 0x10;
    gfx_renderer_.set_palette(palette_data_);
    gfx_renderer_.set_gfx_buffer(gfx_data_.data() + 0x8000, 0x4000);

    obj_renderer_.stride() = 0x20;
    obj_renderer_.priority_mask() = 3;
    obj_renderer_.set_palette(palette_data_);
    obj_renderer_.set_gfx(gfx_data_.data() + 0x30000, 0x10000);
    obj_renderer_.set_obj_buffer(obj_data_);

    chr_renderer_.frame() = {0x100, 0x100};
    chr_renderer_.priority_mask() = 0xFF;
    chr_renderer_.set_palette(palette_data_);
    chr_renderer_.set_gfx(gfx_data_.data() + 0x30000, 0x10000);

    std::vector<uint32_t> chr_tiles(0x20 * 0x20);
    std::vector<VkExtent2D> chr_coords(chr_tiles.size());
    for (size_t i = 0; i < chr_tiles.size(); i++) {
        chr_tiles[i] = static_cast<uint32_t>(i);
        chr_coords[i].width = static_cast<uint32_t>((i & 31) << 3);
        chr_coords[i].height = static_cast<uint32_t>((i >> 2) & 0xFF8);
    }

    chr_renderer_.set_chr_tiles(chr_tiles, chr_coords);

    constexpr static uint32_t src_width = 0x200;
    constexpr static uint32_t dest_width = 0x10 * 0x0D;
    constexpr static uint32_t height = 0x0D;

    std::vector<uint8_t> tiles(height * dest_width);
    for (size_t y = 0; y < height; y++) {
        const uint8_t* src_row = tilemap_data_.data() + (y * src_width);
        uint8_t* dest_row = tiles.data() + (y * dest_width);
        for (size_t x = 0; x < dest_width; x++) {
            dest_row[x] = src_row[x];
        }
    }

    map16_renderer_.stride() = dest_width;
    map16_renderer_.set_palette(palette_data_);
    map16_renderer_.set_gfx(gfx_data_.data() + 0x30000, 0x10000);
    map16_renderer_.set_obj_buffer(obj_data_.data(), 0x400);
    map16_renderer_.set_map16_buffer(tiles);

    glm::vec2 pixels_per_tile = glm::vec2(extent.width, extent.height) / 16.0f;
    glm::vec2 start_tile = glm::vec2(2, 3) * pixels_per_tile;
    glm::vec2 end_tile = glm::vec2(6, 7) * pixels_per_tile;
    glm::vec2 size = end_tile - start_tile;

    glm::vec2 points[] = {
            start_tile,
            glm::vec2(end_tile.x, start_tile.y),
            glm::vec2(end_tile.x, start_tile.y),
            end_tile,
            end_tile,
            glm::vec2(start_tile.x, end_tile.y),
            glm::vec2(start_tile.x, end_tile.y),
            start_tile,
    };

    int frame = static_cast<int>(current_frame());
    float repeat_length = 16;
    float rate = -0.25f;
    float offset = rate * frame / repeat_length;
    float tex_coords[] = {
            offset, offset + (size.x / repeat_length),
            offset, offset + (size.y / repeat_length),
            offset, offset + (size.x / repeat_length),
            offset, offset + (size.y / repeat_length),
    };

    line_renderer_.frame_size() = extent;
    line_renderer_.line_width() = 2;
    line_renderer_.set_vertices(points, tex_coords);
}

void MainWindow::draw() {
    maseya::vksnes::Buffer buffer(easy_api_.allocator(),
                                  VK_BUFFER_USAGE_TRANSFER_DST_BIT, 0x200 * 0x200 * 4);
    while (true) {
        // Attempt to get the image
        std::optional<uint32_t> image_index =
                managed_swapchain_.acquire_next_image(10000);

        // Check for failure
        if (!image_index.has_value()) {
            // This is an indirect way to check for timeout. If the swapchain is still
            // valid, then we failed to get an image due to a timeout. In such a
            // circumstance, we must simple give up drawing this frame.
            if (static_cast<bool>(*managed_swapchain_)) {
                return;
            }

            // If the swapchain, however, is invalid, then we must try recreating it.
            if (recreate_swapchain()) {
                // If recreating it succeeds, then we try again to render the frame.
                continue;
            }

            // If recreation fails, then give up rendering.
            return;
        }

        const SwapchainImage& swapchain_image =
                managed_swapchain_.images()[image_index.value()];

        const FlatFramebuffer& framebuffer =
                framebuffers_[swapchain_image.image_index()];

        const Frame& frame = managed_swapchain_.current_frame();
        frame.wait_for_command();

        glm::vec4 clear_color(0);
        const CommandBuffer& command_buffer = frame.command_buffer();
        command_buffer.begin();

        command_buffer.set_viewport_and_scissor(overlay_framebuffer_.extent());
        command_buffer.begin_flat_render_pass(
                *overlay_render_pass_, *overlay_framebuffer_,
                overlay_framebuffer_.extent(), clear_color);

        obj_renderer_.queue_draw(command_buffer);
        // chr_renderer_.queue_draw(command_buffer);

        command_buffer.end_render_pass();

        VkImageMemoryBarrier image_memory_barrier{};
        image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        image_memory_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_memory_barrier.image = *overlay_image_;
        image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_memory_barrier.subresourceRange.baseMipLevel = 0;
        image_memory_barrier.subresourceRange.levelCount = 1;
        image_memory_barrier.subresourceRange.baseArrayLayer = 0;
        image_memory_barrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(*command_buffer,
                             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                             VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0,
                             nullptr, 1, &image_memory_barrier);

        command_buffer.set_viewport_and_scissor(framebuffer.extent());
        command_buffer.begin_flat_render_pass(*present_render_pass_, *framebuffer,
                                              framebuffer.extent(), clear_color);

        checkerboard_renderer_.queue_draw(command_buffer);
        gradient_renderer_.queue_draw(command_buffer);
        overlay_renderer_.queue_draw(command_buffer);
        line_renderer_.queue_draw(command_buffer);

        command_buffer.end_render_pass();
        command_buffer.end();

        frame.submit(*easy_api_.graphics_queue());

        // We will attempt to present the image to the screen.
        if (easy_api_.presentation_queue().present(swapchain_image,
                                                   frame.command_signal_semaphore())) {
            // If presentation is successful, then drawing is complete and we exit the
            // loop.
            return;
        }

        // Otherwise, we repeat the loop and try again.
        continue;
    }
}

void MainWindow::run() {
    show();

    start_time_ = std::chrono::steady_clock::now();
    WinApiWindow::run();
}

void MainWindow::on_size(WPARAM size_mode, SHORT width, SHORT height) {
    width_ = width;
    height_ = height;

    std::stringstream ss;
    ss << width_ << ", " << height_;
    std::string title(ss.str());
    SetWindowTextA(hwnd(), title.c_str());

    if (is_minimized(extent())) {
        return;
    }

    if (recreate_swapchain()) {
        tick();
    }
}

void MainWindow::on_move(const POINTS& location) {}

void MainWindow::on_mouse_wheel(WORD key_state, WORD z_scroll, WORD x_pos, WORD y_pos) {
}

void MainWindow::on_key_down(WPARAM key) {}

bool MainWindow::recreate_swapchain() {
    if (managed_swapchain_.recreate_swapchain()) {
        framebuffers_ = easy_api_.create_flat_framebuffers(*managed_swapchain_);
        return true;
    }

    return false;
}

double MainWindow::current_frame() const {
    duration elapsed = std::chrono::steady_clock::now() - start_time_;
    return elapsed.count();
}

LRESULT CALLBACK MainWindow::WindowProc(HWND hwnd_, UINT uMsg, WPARAM wParam,
                                        LPARAM lParam) {
    MainWindow& window = get_window(hwnd_);
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

        case WM_MOUSEWHEEL:
            window.on_mouse_wheel(GET_KEYSTATE_WPARAM(wParam),
                                  GET_WHEEL_DELTA_WPARAM(wParam), GET_X_LPARAM(lParam),
                                  GET_Y_LPARAM(lParam));
            return 0;

        case WM_KEYDOWN:
            window.on_key_down(wParam);
            return 0;
    }

    return CallWindowProcW(window.def_wnd_proc_, hwnd_, uMsg, wParam, lParam);
}

MainWindow& get_window(HWND hwnd) {
    return *reinterpret_cast<MainWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
}
}  // namespace maseya::vksnes