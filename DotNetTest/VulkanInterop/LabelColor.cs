namespace DotNetTest.VulkanInterop
{
    using System;
    using System.Drawing;
    using System.Runtime.InteropServices;

    internal struct LabelColor
    {
#pragma warning disable 0649
        public float R, G, B, A;
#pragma warning restore 0649

        public LabelColor(IntPtr ptr, int index)
        {
            this = Marshal.PtrToStructure<LabelColor>(ptr + (index * IntPtr.Size));
        }

        public static implicit operator Color(LabelColor color)
        {
            return Color.FromArgb(
                (int)(color.A * 255),
                (int)(color.R * 255),
                (int)(color.G * 255),
                (int)(color.B * 255));
        }
    }
}
