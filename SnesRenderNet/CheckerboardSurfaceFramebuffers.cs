namespace SnesRenderNet
{
    using System;
    using System.Collections.Generic;
    using static UnsafeNativeMethods;

    public class CheckerboardSurfaceFramebuffers : SurfaceFramebuffers, IDisposable
    {
        private bool disposed_;

        public CheckerboardSurfaceFramebuffers(
            CheckerboardPipeline pipeline,
            Surface surface,
            int finalLayout = 1000001002)
            : base(CreateCheckerboardSurfaceFramebuffers(
                pipeline.Handle,
                surface.Handle,
                finalLayout))
        {
            Framebuffers = new Dictionary<IntPtr, CheckerboardFramebuffer>();
            foreach (var framebuffer in this)
            {
                Framebuffers.Add(
                    framebuffer.Handle,
                    new CheckerboardFramebuffer(framebuffer.Handle));
            }
        }

        ~CheckerboardSurfaceFramebuffers()
        {
            Dispose(false);
        }

        private Dictionary<IntPtr, CheckerboardFramebuffer> Framebuffers { get; }

        public CheckerboardFramebuffer GetFramebuffer(SwapchainImage image)
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

            DestroyCheckerboardSurfaceFramebuffers(Handle);
            disposed_ = true;
        }
    }
}
