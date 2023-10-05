namespace SnesRenderNet.VulkanInterop
{
    using System;

    internal struct DebugUtilsMessengerCallbackData
    {
#pragma warning disable 0649
        public VulkanStructType sType;
        public IntPtr pNext;
        public int flags;
        public IntPtr pMessageIdName;
        public int messageIdNumber;
        public IntPtr pMessage;
        public int queueLabelCount;
        public IntPtr pQueueLabels;
        public int cmdBufLabelCount;
        public IntPtr pCmdBufLabels;
        public int objectCount;
        public IntPtr pObjects;
#pragma warning disable 0649
    }
}
