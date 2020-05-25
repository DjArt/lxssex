using System;

namespace EGL
{
    [AttributeUsage(AttributeTargets.Field, AllowMultiple = false)]
    public sealed class AttributeParameterCountAttribute : Attribute
    {
        public uint Count { get; }

        public AttributeParameterCountAttribute(uint count)
        {
            Count = count;
        }
    }
}