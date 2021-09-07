namespace DotNetTest.VulkanInterop
{
    using System.Runtime.InteropServices;

    internal unsafe delegate bool DebugMessengerCallbackInternal(
        MessageSeverity messageSeverity,
        MessageType messageType,
        DebugUtilsMessengerCallbackData* callbackData,
        GCHandle userData);
}
