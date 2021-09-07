namespace DotNetTest
{
    using System;
    using System.Runtime.InteropServices;
    using System.Windows.Forms;
    using VulkanInterop;

    public class Instance : IDisposable
    {
        private bool disposed_ = false;

        public Instance()
            : this(String.Empty)
        {
        }

        public Instance(string applicationName)
            : this(applicationName, new Version(1))
        {
        }

        public Instance(string applicationName, Version version)
        {
            unsafe
            {
                Handle = UnsafeNativeMethods.CreateInstance(
                    applicationName,
                    &version,
                    null);
            }
        }

        public Instance(
            DebugMessengerCallback debuggMessengerCallback) :
            this(String.Empty, debuggMessengerCallback)
        {
        }

        public Instance(
            string applicationName,
            DebugMessengerCallback debuggMessengerCallback) :
            this(applicationName, new Version(1), debuggMessengerCallback)
        {
        }

        public Instance(
            string applicationName,
            Version version,
            DebugMessengerCallback debuggMessengerCallback)
        {
            DebugMessengerCallback = debuggMessengerCallback;

            unsafe
            {
                var messageSeverity = MessageSeverity.Verbose
                    | MessageSeverity.Warning
                    | MessageSeverity.Error;

                var messageType = MessageType.General
                    | MessageType.Validation
                    | MessageType.Performance;

                // Create an unmanaged pointer to our debug delegate so it can
                // be sent to the Render API.
                DebugCallbackHandle = GCHandle.Alloc(this);

                // We need to actually construct a Delegate object of the
                // static function. Further, it cannot be a temporary as that
                // could allow the GC to destroy it.
                HandleDebugMessageDelegate = HandleDebugMessage;

                var debugMessengerCreateInfo = new DebugMessengerCreateInfo(
                    messageSeverity,
                    messageType,
                    Marshal.GetFunctionPointerForDelegate(HandleDebugMessageDelegate),
                    DebugCallbackHandle);

                Handle = UnsafeNativeMethods.CreateInstance(
                    applicationName,
                    &version,
                    &debugMessengerCreateInfo);
            }
        }

        ~Instance()
        {
            Dispose(false);
        }

        public DebugMessengerCallback DebugMessengerCallback { get; set; }

        internal IntPtr Handle { get; }

        private GCHandle DebugCallbackHandle { get; }

        private DebugMessengerCallbackInternal HandleDebugMessageDelegate { get; }

        public Surface CreateWin32Surface(IWin32Window window)
        {
            return new Surface(UnsafeNativeMethods.CreateWin32Surface(
                Handle,
                window.Handle));
        }

        public void RenderTestImage(RenderTarget renderTarget)
        {
            UnsafeNativeMethods.RenderTestImage(Handle, renderTarget.Handle);
        }

        public void RenderTestImageWithBuffers(
            RenderTarget renderTarget,
            TestVertexBuffers vertexBuffers,
            TestUniformBuffers uniformBuffers,
            float angle)
        {
            UnsafeNativeMethods.RenderTestImageWithBuffers(
                Handle,
                renderTarget.Handle,
                uniformBuffers.Handle,
                vertexBuffers.Handle,
                angle);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposed_)
            {
                return;
            }

            UnsafeNativeMethods.DestroyInstance(Handle);
            DebugCallbackHandle.Free();
            disposed_ = true;
        }

        private static unsafe bool HandleDebugMessage(
            MessageSeverity messageSeverity,
            MessageType messageType,
            DebugUtilsMessengerCallbackData* callbackData,
            GCHandle userData)
        {
            var callback = (userData.Target as Instance)?.DebugMessengerCallback;

            if (callback != null)
            {
                var bufferLabels = new DebugLabel[callbackData->cmdBufLabelCount];
                for (var i = 0; i < bufferLabels.Length; i++)
                {
                    bufferLabels[i] = new DebugLabelInternal(
                        callbackData->pCmdBufLabels,
                        i);
                }

                var queueLabels = new DebugLabel[callbackData->queueLabelCount];
                for (var i = 0; i < queueLabels.Length; i++)
                {
                    queueLabels[i] = new DebugLabelInternal(
                        callbackData->pQueueLabels,
                        i);
                }

                var objects = new DebugObjectNameInfo[callbackData->objectCount];
                for (var i = 0; i < objects.Length; i++)
                {
                    objects[i] = new DebugObjectNameInfoInternal(
                        callbackData->pObjects,
                        i);
                }

                return callback(
                    messageSeverity,
                    messageType,
                    callbackData->messageIdNumber,
                    Marshal.PtrToStringAnsi(callbackData->pMessageIdName),
                    Marshal.PtrToStringAnsi(callbackData->pMessage),
                    bufferLabels,
                    queueLabels,
                    objects);
            }

            return false;
        }
    }
}
