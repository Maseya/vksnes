namespace SnesRenderNet
{
    using System;
    using static UnsafeNativeMethods;

    public class PalettePipeline : IDisposable
    {
        private bool disposed_;

        public PalettePipeline(Instance instance)
        {
            if (instance is null)
            {
                throw new ArgumentNullException(nameof(instance));
            }

            Handle = CreatePalettePipeline(instance.Handle);
            if (Handle == IntPtr.Zero)
            {
                throw new Exception();
            }
        }

        ~PalettePipeline()
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

            DestroyPalettePipeline(Handle);
            disposed_ = true;
        }
    }
}
