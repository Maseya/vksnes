namespace DotNetTest
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
        public static unsafe extern IntPtr CreateInstance(
            [MarshalAs(UnmanagedType.LPStr)]
            string applicationName,
            Version* version,
            DebugMessengerCreateInfo* debug_messenger_create_info);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "destroy_instance",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern void DestroyInstance(IntPtr instance);

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
            EntryPoint = "acquire_render_target_from_surface",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr AcquireRenderTargetFromSurface(
            IntPtr surface,
            [MarshalAs(UnmanagedType.U4)] int timeout);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "destroy_render_target",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern void DestroyRenderTarget(IntPtr renderTarget);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "present_render_target_to_surface",
            CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool PresentRenderTargetToSurface(
            IntPtr surface,
            IntPtr renderTarget);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "render_test_image",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern void RenderTestImage(
            IntPtr instance,
            IntPtr renderTarget);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "create_test_vertex_buffers",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr CreateTestVertexBuffers(IntPtr surface);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "update_vertex_buffers",
            CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.U1)]
        public static extern bool UpdateVertexBuffers(
            IntPtr vertexBuffers,
            IntPtr data,
            IntPtr vertexCount,
            IntPtr vertexSize,
            ulong timeout);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "destroy_test_vertex_buffers",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern void DestroyTestVertexBuffers(IntPtr vertexBuffers);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "create_test_uniform_buffers",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr CreateTestUniformBuffers(IntPtr surface);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "destroy_test_uniform_buffers",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern void DestroyTestUniformBuffers(IntPtr uniformBuffers);

        [DllImport(
            "SnesRenderApi.dll",
            EntryPoint = "render_test_image_with_buffers",
            CallingConvention = CallingConvention.Cdecl)]
        public static extern void RenderTestImageWithBuffers(
            IntPtr instance,
            IntPtr renderTarget,
            IntPtr uniformBuffers,
            IntPtr vertexBuffers,
            float angle);
    }
}
