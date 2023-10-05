namespace SnesRenderNet.VulkanInterop
{
    public class DebugObjectNameInfo
    {
        public DebugObjectNameInfo(ObjectType objectType, ulong handle, string name)
        {
            ObjectType = objectType;
            Handle = handle;
            Name = name;
        }

        public ObjectType ObjectType { get; }

        public ulong Handle { get; }

        public string Name { get; }

        public override string ToString()
        {
            return $"({ObjectType}): {Name ?? "Unnamed"}";
        }
    }
}
