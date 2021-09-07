namespace DotNetTest.VulkanInterop
{
    using System.Collections.Generic;

    public delegate bool DebugMessengerCallback(
        MessageSeverity messageSeverity,
        MessageType messageType,
        int messageID,
        string messageIdName,
        string message,
        DebugLabel[] bufferLabels,
        DebugLabel[] queueLabels,
        DebugObjectNameInfo[] objectInfoList);
}
