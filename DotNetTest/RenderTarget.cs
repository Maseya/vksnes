namespace DotNetTest
{
    using System;

    public class RenderTarget : IDisposable
    {
        private bool disposed_ = false;

        internal RenderTarget(IntPtr handle)
        {
            Handle = handle;
        }

        ~RenderTarget()
        {
            Dispose(false);
        }

        internal IntPtr Handle { get; }

        public static implicit operator bool(RenderTarget renderTarget)
        {
            return renderTarget.Handle != IntPtr.Zero;
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

            UnsafeNativeMethods.DestroyRenderTarget(Handle);
            disposed_ = true;
        }
    }
}
