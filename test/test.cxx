// Local headers
#include "MainWindow.hxx"
#include "WinApiInstance.hxx"
#include "print_debug_message.hxx"

// External APIs
#include <vkbase/read_file.hxx>

// Standard library
#include <iostream>

using namespace maseya::vkbase;
using namespace maseya::vksnes;

int main(int argc, char* argv[]) {
    WinApiInstance instance;

    MainWindow window(instance.create_dummy_hwnd(0x200, 0x200));
    window.palette_data_ = read_file<uint16_t>(argv[1]);
    window.gfx_data_ = read_file<uint8_t>(argv[2]);
    window.obj_data_ = read_file<uint16_t>(argv[3]);
    window.tilemap_data_ = read_file<uint8_t>(argv[4]);
    window.run();

    return 0;
}
