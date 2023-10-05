namespace SnesRenderNet
{
    using System;
    using System.Drawing;
    using static UnsafeNativeMethods;

    public class CheckerboardFramebuffer : FramebufferBase, IDisposable
    {
        private bool disposed_;

        public CheckerboardFramebuffer(
            CheckerboardPipeline pipeline,
            ImageBase image, int finalLayout = 1000001002)
            : base(CreateCheckerboardFramebuffer(
                pipeline.Handle,
                image.Handle,
                finalLayout))
        {
        }

        internal CheckerboardFramebuffer(IntPtr handle)
            : base(handle)
        {
            disposed_ = true;
        }

        ~CheckerboardFramebuffer()
        {
            Dispose(false);
        }

        public void UpdatePattern(SizeF size, Color color1, Color color2)
        {
            UpdatePattern(
                size.Width,
                size.Height,
                color1.A / 255f, color1.R / 255f, color1.G / 255f, color1.B / 255f,
                color2.A / 255f, color2.R / 255f, color2.G / 255f, color2.B / 255f);
        }

        public void UpdatePattern(
            float width,
            float height,
            float a1, float r1, float g1, float b1,
            float a2, float r2, float g2, float b2)
        {
            UpdateCheckerboardPattern(
                Handle,
                width,
                height,
                r1, g1, b1, a1,
                r2, g2, b2, a2);
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

            DestroyCheckerboardFramebuffer(Handle);
            disposed_ = true;
        }
    }
}
