namespace SnesRenderNet
{
    using System;
    using System.Collections;
    using System.Collections.Generic;
    using static UnsafeNativeMethods;

    public class SurfaceFramebuffers : IReadOnlyList<FramebufferBase>
    {
        internal SurfaceFramebuffers(IntPtr handle)
        {
            if (handle == IntPtr.Zero)
            {
                throw new Exception();
            }

            Handle = handle;
        }

        public int Count
        {
            get
            {
                return (int)GetSurfaceFramebuffersSize(Handle);
            }
        }

        internal IntPtr Handle
        {
            get;
        }

        public FramebufferBase this[int index]
        {
            get
            {
                var handle = GetSurfaceFramebufferFromIndex(Handle, (uint)index);
                return handle != IntPtr.Zero
                    ? new FramebufferBase(handle)
                    : throw new Exception();
            }
        }

        public IEnumerator<FramebufferBase> GetEnumerator()
        {
            for (var i = 0; i < Count; i++)
            {
                yield return this[i];
            }
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        protected FramebufferBase GetFramebufferBaseFromImage(SwapchainImage image)
        {
            return image is null
                ? throw new ArgumentNullException(nameof(image))
                : new FramebufferBase(
                    GetSurfaceFramebufferFromImage(Handle, image.Handle));
        }
    }
}
