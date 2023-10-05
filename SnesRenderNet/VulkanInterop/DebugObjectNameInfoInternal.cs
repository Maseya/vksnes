namespace SnesRenderNet.VulkanInterop
{
    using System;
    using System.Runtime.InteropServices;

    internal struct DebugObjectNameInfoInternal
    {
#pragma warning disable 0649
        public VulkanStructType sType;
        public IntPtr pNext;
        public ObjectType objectType;
        public ulong objectHandle;
        public IntPtr pObjectName;
#pragma warning disable 0649

        public DebugObjectNameInfoInternal(IntPtr ptr, int index)
        {
            this = Marshal.PtrToStructure<DebugObjectNameInfoInternal>(
                ptr + (index * IntPtr.Size));
        }

        public static implicit operator DebugObjectNameInfo(
            DebugObjectNameInfoInternal objectInfo)
        {
            return new DebugObjectNameInfo(
                objectInfo.objectType,
                objectInfo.objectHandle,
                Marshal.PtrToStringAnsi(objectInfo.pObjectName));
        }
    }
}
