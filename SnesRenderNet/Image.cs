namespace SnesRenderNet
{
    using System;
    using System.Drawing;
    using System.Runtime.InteropServices;
    using static UnsafeNativeMethods;

    public class Image : ImageBase, IDisposable
    {
        private bool disposed_;

        public Image(Instance instance, Size size)
            : this(instance, size, IntPtr.Zero, 0)
        {
        }

        public Image(Instance instance, Size size, IntPtr pixels, int length)
            : base(CreateImage(
                instance.Handle,
                (uint)size.Width,
                (uint)size.Height,
                pixels,
                (uint)length))
        {
        }

        ~Image()
        {
            Dispose(false);
        }

        public static Image FromPixelArray<T>(Instance instance, Size size, T[] pixels)
                    where T : unmanaged
        {
            if (pixels is null)
            {
                throw new ArgumentNullException(nameof(pixels));
            }

            unsafe
            {
                fixed (T* ptr = pixels)
                {
                    return new Image(
                        instance,
                        size,
                        (IntPtr)ptr,
                        pixels.Length * Marshal.SizeOf<T>());
                }
            }
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

            DestroyImage(Handle);
            disposed_ = true;
        }
    }
}
