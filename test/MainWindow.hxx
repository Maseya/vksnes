#pragma once

// Local headers
#include "WinApiInstance.hxx"
#include "WinApiWindow.hxx"

// external APIs
#include <SnesRenderApi/Instance.hxx>
#include <SnesRenderApi/Surface.hxx>
#include <SnesRenderApi/Vertex.hxx>
#include <SnesRenderApi/VertexBufferHelper.hxx>
#include <SnesRenderApi/vma_unique_handles.hxx>

namespace maseya::renderer {
class MainWindow : public WinApiWindow {
private:
    constexpr static Vertex vertices[] = {{+0.0f, -0.5f, 1.0f, 1.0f, 0.0f},
                                          {+0.5f, 0.5f, 0.0f, 1.0f, 1.0f},
                                          {-0.5f, 0.5f, 1.0f, 0.0f, 1.0f}};

    constexpr static Vertex vertices2[] = {
            {-0.5f, -0.5f, 1.0f, 1.0f, 0.0f}, {+0.5f, -0.5f, 0.0f, 1.0f, 1.0f},
            {+0.5f, +0.5f, 1.0f, 0.0f, 1.0f}, {-0.5f, -0.5f, 1.0f, 0.0f, 1.0f},
            {+0.5f, +0.5f, 1.0f, 0.0f, 1.0f}, {-0.5f, +0.5f, 1.0f, 0.0f, 1.0f}};

    MainWindow(WinApiInstance&& win_api_instance);

public:
    MainWindow() : MainWindow(WinApiInstance()) {}

    MainWindow(const MainWindow&) = delete;
    MainWindow(MainWindow&&) noexcept = default;

    virtual ~MainWindow();

    MainWindow& operator=(const MainWindow&) = delete;
    MainWindow& operator=(MainWindow&&) noexcept = default;

    void tick();

    void draw();

    virtual void run();

    virtual void on_size(WPARAM size_mode, SHORT width, SHORT height);
    virtual void on_move(const POINTS& location);

private:
    static MainWindow& get_window(HWND hwnd);

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                                       LPARAM lParam);

private:
    WNDPROC def_wnd_proc_;
    WinApiInstance win_api_instance_;
    Instance instance_;
    Surface surface_;
    TestUniformBuffers uniform_buffers_;
    VertexBufferHelper vertex_buffers_;
    int frame_;
};
}  // namespace maseya::renderer
