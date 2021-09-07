namespace DotNetTest.VulkanInterop
{
    using System;
    using System.Runtime.InteropServices;

    internal struct DebugLabelInternal
    {
#pragma warning disable 0649
        public VulkanStructType sType;
        public IntPtr pNext;
        public IntPtr pLabelName;
        public LabelColor color;
#pragma warning restore 0649

        public DebugLabelInternal(IntPtr ptr, int index)
        {
            this = Marshal.PtrToStructure<DebugLabelInternal>(
                ptr + (index * IntPtr.Size));
        }

        public static implicit operator DebugLabel(DebugLabelInternal label)
        {
            return new DebugLabel(
                Marshal.PtrToStringAnsi(label.pLabelName),
                label.color);
        }
    }
}
