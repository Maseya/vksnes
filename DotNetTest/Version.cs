namespace DotNetTest
{
    public struct Version
    {
        public Version(uint major, uint minor = 0, uint patch = 0)
        {
            Major = major;
            Minor = minor;
            Patch = patch;
        }

        public uint Major { get; set; }

        public uint Minor { get; set; }

        public uint Patch { get; set; }

        public override string ToString()
        {
            return $"v{Major}.{Minor}.{Patch}";
        }
    }
}
