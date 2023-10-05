namespace SnesRenderNet
{
    using System;
    using System.Windows.Forms;
    using static UnsafeNativeMethods;

    public class Surface : IDisposable
    {
        private bool disposed_ = false;

        public Surface(Instance instance, IWin32Window window)
        {
            if (instance is null)
            {
                throw new ArgumentNullException(nameof(instance));
            }

            if (window is null)
            {
                throw new ArgumentNullException(nameof(window));
            }

            Handle = CreateWin32Surface(instance.Handle, window.Handle);

            if (Handle == IntPtr.Zero)
            {
                throw new Exception();
            }
        }

        ~Surface()
        {
            Dispose(false);
        }

        internal IntPtr Handle { get; }

        public bool IsSwapchainValid()
        {
            return UnsafeNativeMethods.IsSwapchainValid(Handle);
        }

        public bool TryGetSurfaceImage(
            out SwapchainImage image,
            ulong timeout = Instance.NoTimeout)
        {
            var handle = AcquireSurfaceImage(Handle, timeout);
            image = handle != IntPtr.Zero ? new SwapchainImage(handle) : null;
            return image != null;
        }

        public bool Present(SwapchainImage image)
        {
            return image is null
                ? throw new ArgumentNullException(nameof(image))
                : PresentImageToSurface(Handle, image.Handle);
        }

        public bool TryRecreateSwapchain()
        {
            return UnsafeNativeMethods.TryRecreateSwapchain(Handle);
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

            DestroySurface(Handle);
            disposed_ = true;
        }
    }
}
