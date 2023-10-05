#pragma once

namespace maseya::vksnes {
enum class PixelFormat {
    Format1Bpp8x8 = 0x11,
    Format2BppNes = 0x22,
    // GameBoy and 2BPP for SNES
    Format2BppGb = 0x32,
    // Neo-Geo Pocket
    Format2BppNgp = 0x42,
    // Virtual Boy
    Format2BppVb = 0x52,
    Format3BppSnes = 0x03,
    Format3Bpp8x8 = 0x13,
    Format4BppSnes = 0x04,
    // GameBoy Advance
    Format4BppGba = 0x14,
    Format4BppSms = 0x24,
    Format4BppMsx2 = 0x34,
    Format4Bpp8x8 = 0x44,
    Format8BppSnes = 0x08,
    // SNES Mode7 (direct copy)
    Format8BppMode7 = 0x78,
};
}