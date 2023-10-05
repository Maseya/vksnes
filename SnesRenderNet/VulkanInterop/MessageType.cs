namespace SnesRenderNet.VulkanInterop
{
    using System;

    [Flags]
    public enum MessageType
    {
        General = 1,
        Validation = 2,
        Performance = 4,
    }
}
