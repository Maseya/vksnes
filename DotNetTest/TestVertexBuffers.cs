namespace DotNetTest
{
    using System;
    using System.Runtime.InteropServices;

    public class TestVertexBuffers : IDisposable
    {
        private bool disposed_;

        internal TestVertexBuffers(IntPtr handle)
        {
            Handle = handle;
        }

        ~TestVertexBuffers()
        {
            Dispose(false);
        }

        internal IntPtr Handle { get; }

        public bool UpdateBuffer<T>(
            T[] data,
            ulong timeout = Int64.MaxValue)
            where T : unmanaged
        {
            return UpdateBuffer(data, Marshal.SizeOf<T>(), timeout);
        }

        public bool UpdateBuffer<T>(
            T[] data,
            int vertex_size,
            ulong timeout = Int64.MaxValue)
            where T : unmanaged
        {
            unsafe
            {
                fixed (T* ptr = data)
                {
                    return UnsafeNativeMethods.UpdateVertexBuffers(
                        Handle,
                        (IntPtr)ptr,
                        (IntPtr)data.LongLength,
                        (IntPtr)vertex_size,
                        timeout);
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

            UnsafeNativeMethods.DestroyTestVertexBuffers(Handle);
            disposed_ = true;
        }
    }
}
