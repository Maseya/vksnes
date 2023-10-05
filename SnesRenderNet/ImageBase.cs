namespace SnesRenderNet
{
    using System;
    using static UnsafeNativeMethods;

    public class ImageBase
    {
        private int layout_;

        internal ImageBase(IntPtr handle)
        {
            if (handle == IntPtr.Zero)
            {
                throw new Exception();
            }

            Handle = handle;
        }

        public int Layout
        {
            get
            {
                return layout_;
            }

            set
            {
                layout_ = value;
                SetImageLayout(Handle, value);
            }
        }

        internal IntPtr Handle
        {
            get;
        }
    }
}
