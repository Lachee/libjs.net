using System.Diagnostics;
using System.Runtime.InteropServices;

namespace LibJS
{

    public class Object : IUnmanagedObject
    {

        [DllImport(Document.LibraryName)] protected static extern ulong js_object_get_property_value_at_index(IntPtr obj, int index);
        [DllImport(Document.LibraryName)] protected static extern ulong js_object_get_property_value(IntPtr obj, string name);

        private IntPtr m_ptr;
        public IntPtr Ptr => m_ptr;

        internal Object(IntPtr ptr)
        {
            Debug.Assert(ptr != IntPtr.Zero);
            m_ptr = ptr;
        }

        public Value GetProperty(int index)
        {
            var property = js_object_get_property_value_at_index(m_ptr, index);

            var value = new Value(property);
            if (value.IsUndefined)
            {
                throw new IndexUndefinedException(index);
            }

            return value;
        }

        public Value GetProperty(string name)
        {
            var property = js_object_get_property_value(m_ptr, name);

            var value = new Value(property);
            if (value.IsUndefined)
            {
                throw new PropertyUndefinedException(name);
            }

            return value;
        }

        public void Dispose()
        {
        }
    }

}