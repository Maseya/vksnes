namespace SnesRenderNet
{
    using System;
    using System.Drawing;
    using System.Runtime.InteropServices;
    using static UnsafeNativeMethods;

    public class PaletteFramebuffer : FramebufferBase, IDisposable
    {
        private bool disposed_;

        public PaletteFramebuffer(
            PalettePipeline pipeline,
            ImageBase image, int finalLayout = 1000001002)
            : base(CreatePaletteFramebuffer(
                pipeline.Handle,
                image.Handle,
                finalLayout))
        {
        }

        internal PaletteFramebuffer(IntPtr handle)
            : base(handle)
        {
            disposed_ = true;
        }

        ~PaletteFramebuffer()
        {
            Dispose(false);
        }

        public void UpdateColorData<T>(T[] data) where T : unmanaged
        {
            if (data is null)
            {
                throw new ArgumentNullException(nameof(data));
            }

            unsafe
            {
                fixed (T* ptr = data)
                {
                    UpdateColorData((IntPtr)ptr, data.Length * Marshal.SizeOf<T>());
                }
            }
        }

        public void UpdateColorData(IntPtr data, int size)
        {
            UpdatePaletteColorData(Handle, data, (uint)size);
        }

        public void UpdateView(Size size)
        {
            UpdateView(size.Width, size.Height);
        }

        public void UpdateView(int width, int height)
        {
            UpdatePaletteView(Handle, (uint)width, (uint)height);
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

            DestroyPaletteFramebuffer(Handle);
            disposed_ = true;
        }
    }
}
