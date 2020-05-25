using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;

namespace EGL
{
    public abstract class EGLAttributeList<T>
        where T : Enum
    {
        protected abstract T ListStop { get; }

        public static implicit operator T[](EGLAttributeList<T> attributes)
        {
            List<T> result = new List<T>();
            string[] availableNames = Enum.GetNames(typeof(T));
            IEnumerable<PropertyInfo> props = attributes.GetType().GetTypeInfo().DeclaredProperties.Where(x => availableNames.Contains(x.Name));
            foreach (PropertyInfo prop in props)
            {
                T attribute = (T)Enum.Parse(typeof(T), prop.Name);
                object value = prop.GetValue(attributes);
                AttributeParameterCountAttribute attr = typeof(T).GetTypeInfo().GetDeclaredField(prop.Name).GetCustomAttribute<AttributeParameterCountAttribute>();
                if (attr != null)
                {
                    switch (attr.Count)
                    {
                        case 0:
                            if (prop.PropertyType != typeof(bool))
                            {
                                throw new ArgumentOutOfRangeException(prop.Name, $"{nameof(AttributeParameterCountAttribute)} with Count = 0 requires type of property be bool");
                            }
                            if ((bool)value)
                            {
                                result.Add(attribute);
                            }
                            break;
                        case 1:
                            if (Nullable.GetUnderlyingType(prop.PropertyType) == null)
                            {
                                throw new ArgumentOutOfRangeException(prop.Name, $"{nameof(AttributeParameterCountAttribute)} with Count = 1 requires type of property be Nullable<T>");
                            }
                            if (value != null)
                            {
                                result.Add(attribute);
                                result.Add((T)value);
                            }
                            break;
                        default:
                            throw new NotImplementedException($"Attribute {prop.Name} in {typeof(T).FullName} has more than 1 values, that's not implemented!");
                    }
                }
                else
                {
                    throw new NotImplementedException($"Attribute {prop.Name} in {typeof(T).FullName} doesn't contains required {nameof(AttributeParameterCountAttribute)} attribute!");
                }
            }
            result.Add((T)Enum.ToObject(typeof(T), attributes.ListStop));
            return result.ToArray();
        }

        protected void FillProperties(T[] attributes)
        {
            string[] availableNames = Enum.GetNames(typeof(T));
            PropertyInfo[] props = attributes.GetType().GetTypeInfo().DeclaredProperties.Where(x => availableNames.Contains(x.Name)).ToArray();
            for (int i0 = 0; i0 < attributes.Length; i0++)
            {
                T attribute = attributes[i0];
                if (!Equals(attribute, ListStop))
                {
                    PropertyInfo prop = props.First(x => x.Name == Enum.GetName(typeof(T), attribute));
                    AttributeParameterCountAttribute attr = typeof(T).GetTypeInfo().GetDeclaredField(prop.Name).GetCustomAttribute<AttributeParameterCountAttribute>();
                    if (attr != null)
                    {
                        switch (attr.Count)
                        {
                            case 0:
                                if (prop.PropertyType != typeof(bool)) throw new ArgumentOutOfRangeException(prop.Name, $"{nameof(AttributeParameterCountAttribute)} with Count = 0 requires type of property be bool");
                                prop.SetValue(this, true);
                                break;
                            case 1:
                                if (Nullable.GetUnderlyingType(prop.PropertyType) == null)
                                {
                                    throw new ArgumentOutOfRangeException(prop.Name, $"{nameof(AttributeParameterCountAttribute)} with Count = 1 requires type of property be Nullable<T>");
                                }
                                prop.SetValue(this, attributes[++i0]);
                                break;
                            default:
                                throw new NotImplementedException($"Attribute {prop.Name} in {typeof(T).FullName} has more than 1 values, that's not implemented!");
                        }
                    }
                    else
                    {
                        throw new NotImplementedException($"Attribute {prop.Name} in {typeof(T).FullName} doesn't contains required {nameof(AttributeParameterCountAttribute)} attribute!");
                    }
                }
            }
        }
    }
}
