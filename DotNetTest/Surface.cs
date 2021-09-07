namespace DotNetTest
{
    using System;

    public class Surface : IDisposable
    {
        private bool disposed_ = false;

        internal Surface(IntPtr handle)
        {
            Handle = handle;
        }

        ~Surface()
        {
            Dispose(false);
        }

        internal IntPtr Handle { get; }

        public RenderTarget AcquireRenderTarget()
        {
            return new RenderTarget(
                UnsafeNativeMethods.AcquireRenderTargetFromSurface(Handle, -1));
        }

        public TestVertexBuffers CreateTestVertexBuffers()
        {
            return new TestVertexBuffers(
                UnsafeNativeMethods.CreateTestVertexBuffers(Handle));
        }

        public TestUniformBuffers CreateTestUniformBuffers()
        {
            return new TestUniformBuffers(
                UnsafeNativeMethods.CreateTestUniformBuffers(Handle));
        }

        public bool Present(RenderTarget renderTarget)
        {
            return UnsafeNativeMethods.PresentRenderTargetToSurface(
                Handle,
                renderTarget.Handle);
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

            UnsafeNativeMethods.DestroySurface(Handle);
            disposed_ = true;
        }
    }
}
