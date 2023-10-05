namespace SnesRenderNet
{
    using System;
    using System.Runtime.InteropServices;
    using VulkanInterop;

    internal static class UnsafeNativeMethods
    {
        [DllImport(
            "SnesRenderApi.dll",
            CharSet = CharSet.Ansi,
            EntryPoint = "create_instance",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern unsafe IntPtr CreateInstance(
            [MarshalAs(UnmanagedType.LPStr)]
            string applicationName,
            [MarshalAs(UnmanagedType.U4)]
            int version,
            DebugMessengerCreateInfo* debug_messenger_create_info);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "destroy_instance",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern void DestroyInstance(IntPtr instance);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "instance_wait_idle",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern void InstanceWaitIdle(IntPtr instance);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "create_win32_surface",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr CreateWin32Surface(
            IntPtr instance,
            IntPtr hwnd);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "destroy_surface",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern void DestroySurface(IntPtr surface);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "is_swapchain_valid",
            CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool IsSwapchainValid(IntPtr surface);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "present_image_to_surface",
            CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool PresentImageToSurface(IntPtr surface, IntPtr image);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "acquire_surface_image",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr AcquireSurfaceImage(
            IntPtr surface,
            [MarshalAs(UnmanagedType.U8)]
            ulong timeout);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "try_recreate_swapchain",
            CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool TryRecreateSwapchain(IntPtr surface);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "create_image",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr CreateImage(
            IntPtr instance,
            [MarshalAs(UnmanagedType.U4)]
            uint width,
            [MarshalAs(UnmanagedType.U4)]
            uint height,
            IntPtr pixels,
            [MarshalAs(UnmanagedType.U4)]
            uint size);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "destroy_image",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern void DestroyImage(IntPtr image);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "set_image_layout",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetImageLayout(
            IntPtr image,
            [MarshalAs(UnmanagedType.I4)]
            int layout);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "wait_framebuffer_idle",
            CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool WaitFramebufferIdle(
            IntPtr framebuffer,
            [MarshalAs(UnmanagedType.U8)]
            ulong timeout);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "set_framebuffer_dependency",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetFramebufferDependency(
            IntPtr owner,
            IntPtr dependency);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "framebuffer_has_dependency",
            CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool FramebufferHasDependency(IntPtr framebuffer);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "render_framebuffer",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern void RenderFramebuffer(
            IntPtr framebuffer,
            [MarshalAs(UnmanagedType.U4)]
            int instanceCount);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "create_checkerboard_pipeline",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr CreateCheckerboardPipeline(IntPtr instance);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "destroy_checkerboard_pipeline",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern void DestroyCheckerboardPipeline(IntPtr pipeline);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "create_checkerboard_framebuffer",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr CreateCheckerboardFramebuffer(
            IntPtr pipeline,
            IntPtr image,
            int finalLayout);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "destroy_checkerboard_framebuffer",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern void DestroyCheckerboardFramebuffer(IntPtr framebuffer);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "update_checkerboard_pattern",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern void UpdateCheckerboardPattern(
            IntPtr framebuffer,
            float width,
            float height,
            float a1, float r1, float g1, float b1,
            float a2, float r2, float g2, float b2);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "create_palette_pipeline",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr CreatePalettePipeline(IntPtr instance);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "destroy_checkerboard_pipeline",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern void DestroyPalettePipeline(IntPtr pipeline);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "create_palette_framebuffer",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr CreatePaletteFramebuffer(
            IntPtr pipeline,
            IntPtr image,
            int finalLayout);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "destroy_palette_framebuffer",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern void DestroyPaletteFramebuffer(IntPtr framebuffer);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "update_palette_color_data",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern void UpdatePaletteColorData(
            IntPtr framebuffer,
            IntPtr color_data,
            [MarshalAs(UnmanagedType.U4)]
            uint size);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "update_palette_view",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern void UpdatePaletteView(
            IntPtr framebuffer,
            [MarshalAs(UnmanagedType.U4)]
            uint width,
            [MarshalAs(UnmanagedType.U4)]
            uint height);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "get_surface_framebuffer_from_index",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr GetSurfaceFramebufferFromIndex(
            IntPtr surfaceFramebuffers,
            uint index);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "get_surface_framebuffer_from_image",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr GetSurfaceFramebufferFromImage(
            IntPtr surfaceFramebuffers,
            IntPtr image);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "get_surface_framebuffers_size",
            CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.U4)]
        public static extern uint GetSurfaceFramebuffersSize(
            IntPtr surfaceFramebuffers);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "create_checkerboard_surface_framebuffers",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr CreateCheckerboardSurfaceFramebuffers(
            IntPtr pipeline,
            IntPtr surface,
            [MarshalAs(UnmanagedType.I4)]
            int finalLayout);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "destroy_checkerboard_surface_framebuffers",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern void DestroyCheckerboardSurfaceFramebuffers(
            IntPtr surface_framebuffers);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "create_palette_surface_framebuffers",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr CreatePaletteSurfaceFramebuffers(
            IntPtr pipeline,
            IntPtr surface,
            int finalLayout);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "destroy_palette_surface_framebuffers",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern void DestroyPaletteSurfaceFramebuffers(
            IntPtr surface_framebuffers);
    }
}
