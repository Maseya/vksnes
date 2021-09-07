namespace DotNetTest.VulkanInterop
{
    using System;

    [Flags]
    public enum MessageSeverity
    {
        Verbose = 1,
        Info = 0x10,
        Warning = 0x100,
        Error = 0x1000,
    }
}
