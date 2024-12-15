using System.Diagnostics;
using System.Runtime.InteropServices;

namespace LibJS
{

    public class Object : IDisposable
    {

        [DllImport(Environment.LibraryName)] protected static extern IntPtr js_object_get_property_value_at_index(IntPtr obj, int index);
        [DllImport(Environment.LibraryName)] protected static extern IntPtr js_object_get_property_value(IntPtr obj, string name);

        protected IntPtr m_ptr;

        internal Object(IntPtr ptr)
        {
            Debug.Assert(ptr != IntPtr.Zero);
            m_ptr = ptr;
        }

        public Value GetProperty(int index)
        {
            var property = js_object_get_property_value_at_index(m_ptr, index);
            if (property == IntPtr.Zero)
                throw new NullReferenceException();

            var value = new Value(property);
            if (value.IsUndefined)
            {
                value.Dispose();
                throw new IndexUndefinedException(index);
            }

            return value;
        }

        public Value GetProperty(string name)
        {
            var property = js_object_get_property_value(m_ptr, name);
            if (property == IntPtr.Zero)
                throw new NullReferenceException();

            var value = new Value(property);
            if (value.IsUndefined)
            {
                value.Dispose();
                throw new PropertyUndefinedException(name);
            }

            return value;
        }

        public void Dispose()
        {
        }
    }

}