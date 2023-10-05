namespace DotNetTest;

using SnesRenderNet;
using SnesRenderNet.VulkanInterop;

internal static class Program
{
    private static readonly bool Verbose = false;

    /// <summary>
    ///  The main entry point for the application.
    /// </summary>
    [STAThread]
    static void Main()
    {
        // To customize application configuration such as set high DPI settings or default font,
        // see https://aka.ms/applicationconfiguration.
        ApplicationConfiguration.Initialize();

        using var instance = new Instance(DebugMessengerCallback);
        using var form = new MainForm(instance);
        Application.Run(form);
    }

    private static bool DebugMessengerCallback(
        MessageSeverity messageSeverity,
        MessageType messageType,
        int messageID,
        string messageIdName,
        string message,
        DebugLabel[] bufferLabels,
        DebugLabel[] queueLabels,
        DebugObjectNameInfo[] objectInfoList)
    {
        foreach (var label in bufferLabels)
        {
            Console.WriteLine(label);
        }

        foreach (var label in queueLabels)
        {
            Console.WriteLine(label);
        }

        if (messageSeverity > MessageSeverity.Verbose)
        {
            var defaultColor = Console.ForegroundColor;
            Console.ForegroundColor = ConsoleColor.Red;
            Console.Error.WriteLine(message);
            Console.ForegroundColor = defaultColor;
        }
        else if (Verbose)
        {
            Console.WriteLine(message);
        }

        return false;
    }
}