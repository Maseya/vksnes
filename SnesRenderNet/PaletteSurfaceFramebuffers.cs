namespace SnesRenderNet
{
    using System;
    using System.Collections.Generic;
    using static UnsafeNativeMethods;

    public class PaletteSurfaceFramebuffers : SurfaceFramebuffers, IDisposable
    {
        private bool disposed_;

        public PaletteSurfaceFramebuffers(
            PalettePipeline pipeline,
            Surface surface,
            int finalLayout = 1000001002)
            : base(CreatePaletteSurfaceFramebuffers(
                pipeline.Handle,
                surface.Handle,
                finalLayout))
        {
            Framebuffers = new Dictionary<IntPtr, PaletteFramebuffer>();
            foreach (var framebuffer in this)
            {
                Framebuffers.Add(
                    framebuffer.Handle,
                    new PaletteFramebuffer(framebuffer.Handle));
            }
        }

        ~PaletteSurfaceFramebuffers()
        {
            Dispose(false);
        }

        private Dictionary<IntPtr, PaletteFramebuffer> Framebuffers { get; }

        public PaletteFramebuffer GetFramebuffer(SwapchainImage image)
        {
            return Framebuffers[GetFramebufferBaseFromImage(image).Handle];
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposed_)
            {
                return;
            }

            DestroyPaletteSurfaceFramebuffers(Handle);
            disposed_ = true;
        }
    }
}
