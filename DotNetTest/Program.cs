namespace DotNetTest
{
    using System;
    using System.Windows.Forms;
    using VulkanInterop;

    static class Program
    {
        public static bool Verbose = false;

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        private static unsafe void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            using (var instance = new Instance(DebugMessengerCallback))
            using (var form = new MainForm(instance))
            {
                Application.Run(form);
            }
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
                Console.Error.WriteLine(message);
            }
            else if (Verbose)
            {
                Console.WriteLine(message);
            }

            return false;
        }
    }
}
