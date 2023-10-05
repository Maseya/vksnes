namespace SnesRenderNet
{
    using System;
    using static UnsafeNativeMethods;

    public class FramebufferBase
    {
        internal FramebufferBase(IntPtr handle)
        {
            if (handle == IntPtr.Zero)
            {
                throw new Exception();
            }

            Handle = handle;
        }

        public bool IsIdle
        {
            get
            {
                return WaitIdle(0);
            }
        }

        public bool HasDependency
        {
            get
            {
                return FramebufferHasDependency(Handle);
            }
        }

        internal IntPtr Handle
        {
            get;
        }

        public bool WaitIdle(ulong timeout = Instance.NoTimeout)
        {
            return WaitFramebufferIdle(Handle, timeout);
        }

        public void SetDependency(FramebufferBase dependency)
        {
            if (dependency is null)
            {
                throw new ArgumentNullException(nameof(dependency));
            }

            SetFramebufferDependency(Handle, dependency.Handle);
        }

        public void Render(int instanceCount)
        {
            RenderFramebuffer(Handle, instanceCount);
        }
    }
}
