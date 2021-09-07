namespace DotNetTest.VulkanInterop
{
    using System;
    using System.Runtime.InteropServices;

    internal struct DebugMessengerCreateInfo
    {
        public VulkanStructType sType;
        public IntPtr pNext;
        public uint flags;
        public MessageSeverity messageSeverity;
        public MessageType messageType;
        public IntPtr userCallback;
        public GCHandle userData;

        public DebugMessengerCreateInfo(
            MessageSeverity messageSeverity,
            MessageType messageType,
            IntPtr userCallback,
            GCHandle userData)
        {
            sType = VulkanStructType.DebugUtilsMessengerCreateInfo;
            pNext = IntPtr.Zero;
            flags = 0;
            this.messageSeverity = messageSeverity;
            this.messageType = messageType;
            this.userCallback = userCallback;
            this.userData = userData;
        }
    }
}
