namespace DotNetTest.VulkanInterop
{
    using System;
    using System.Drawing;

    public class DebugLabel
    {
        public DebugLabel(string name, Color color)
        {
            Name = name;
            Color = color;
        }

        public string Name { get; }

        public Color Color { get; }

        public override string ToString()
        {
            return Name;
        }
    }
}
