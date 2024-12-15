
using System.Runtime.InteropServices;

namespace LibJS {
    public class Object : IDisposable {

        [DllImport(Environment.LibraryName)] protected static extern IntPtr js_object_get_property_value_at_index(IntPtr obj, int index);
        [DllImport(Environment.LibraryName)] protected static extern IntPtr js_object_get_property_value(IntPtr obj, string name);

        protected IntPtr m_ptr;

        internal Object(IntPtr ptr)
        {
            m_ptr = ptr;
        }

        public Value GetProperty(int index)
        {
            return new Value(js_object_get_property_value_at_index(m_ptr, index));
        }

        public Value GetProperty(string name) {
            return new Value(js_object_get_property_value(m_ptr, name));
        }

        public void Dispose()
        {
        }
    }

    public sealed class Array : Object
    {
        public int Count => GetProperty("length").asInt32();
        public Value this[int index] => GetProperty(index);

        internal Array(nint ptr) : base(ptr)
        {
        }
    }
}