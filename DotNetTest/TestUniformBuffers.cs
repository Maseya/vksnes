namespace DotNetTest
{
    using System;

    public class TestUniformBuffers : IDisposable
    {
        private bool disposed_;

        internal TestUniformBuffers(IntPtr handle)
        {
            Handle = handle;
        }

        ~TestUniformBuffers()
        {
            Dispose(false);
        }

        internal IntPtr Handle { get; }

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

            UnsafeNativeMethods.DestroyTestUniformBuffers(Handle);
            disposed_ = true;
        }
    }
}
