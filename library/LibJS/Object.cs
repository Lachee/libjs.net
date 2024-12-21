using System.Diagnostics;
using System.Runtime.InteropServices;

namespace LibJS
{
    public class Object 
    {
        [DllImport(Consts.LibraryName)] protected static extern ulong js_object_get_property_value_at_index(ulong value, int index);
        [DllImport(Consts.LibraryName)] protected static extern ulong js_object_get_property_value(ulong value, string name);
        [DllImport(Consts.LibraryName)] private static extern ulong js_object_create_error(string message);

        private Value m_value;
		protected internal UIntPtr Ptr => m_value.AsPtr();
        public Value Value => m_value;

		internal Object(Value value) 
        {
            m_value = value;
		}

        [System.Obsolete("Use Value to wrap pointers")]
        internal Object(UIntPtr ptr)
            : this(new Value(Tags.OBJECT_TAG, ptr.ToUInt64())) { }

		public Value GetProperty(int index)
        {
            var property = js_object_get_property_value_at_index(m_value.Encoded, index);
            var value = new Value(property);
            if (value.IsUndefined)
                throw new IndexUndefinedException(index);

            return value;
        }

        public Value GetProperty(string name)
        {
            var property = js_object_get_property_value(m_value.Encoded, name);
            var value = new Value(property);
            if (value.IsUndefined)
                throw new PropertyUndefinedException(name);

            return value;
        }

        public static Object Create(Exception exception) 
        {
            var value = new Value(js_object_create_error($"{exception.GetType().FullName}: {exception.Message}"));
            return new Object(value);
        }
    }
}