namespace SnesRenderNet
{
    using System;
    using static UnsafeNativeMethods;

    public class CheckerboardPipeline : IDisposable
    {
        private bool disposed_;

        public CheckerboardPipeline(Instance instance)
        {
            if (instance is null)
            {
                throw new ArgumentNullException(nameof(instance));
            }

            Handle = CreateCheckerboardPipeline(instance.Handle);
            if (Handle == IntPtr.Zero)
            {
                throw new Exception();
            }
        }

        ~CheckerboardPipeline()
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

            DestroyCheckerboardPipeline(Handle);
            disposed_ = true;
        }
    }
}
