using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;


namespace LibJS
{
    public sealed class Value : IDisposable
    {
        [DllImport(Environment.LibraryName)] static extern void js_value_free(IntPtr value);
        [DllImport(Environment.LibraryName)] static extern ushort js_value_tag(IntPtr value);
        [DllImport(Environment.LibraryName)] static extern ushort js_value_encoded(IntPtr value);
        [DllImport(Environment.LibraryName)] static extern IntPtr js_value_to_string(IntPtr value, [MarshalAs(UnmanagedType.LPStr)] StringBuilder result, UIntPtr result_size);
        [DllImport(Environment.LibraryName)] static extern double js_value_as_double(IntPtr value);
        [DllImport(Environment.LibraryName)] static extern bool js_value_is_function(IntPtr value);
        [DllImport(Environment.LibraryName)] static extern bool js_value_is_constructor(IntPtr value);
        [DllImport(Environment.LibraryName)] static extern bool js_value_is_error(IntPtr value);

        private IntPtr m_ptr;

        private ushort? m_tag;
        private ushort Tag => m_tag ??= js_value_tag(m_ptr);

        private ulong? m_encoded;
        public ulong Encoded => m_encoded ??= js_value_encoded(m_ptr);

        public bool IsAccessor => Tag == Flags.ACCESSOR_TAG;
        public bool IsBigInt => Tag == Flags.BIGINT_TAG;
        public bool IsBoolean => Tag == Flags.BOOLEAN_TAG;
        public bool IsContructor => js_value_is_constructor(m_ptr);
        public bool IsDouble => (Encoded & Flags.CANON_NAN_BITS) != Flags.CANON_NAN_BITS || (Encoded == Flags.CANON_NAN_BITS);
        public bool IsEmpty => Tag == Flags.EMPTY_TAG;
        public bool IsError => js_value_is_error(m_ptr);
        public bool IsFiniteNumber => IsNumber && (IsInt32 || (!IsNaN && !IsInfinity));
        public bool IsFunction => js_value_is_error(m_ptr);
        public bool IsInfinity => (0x1UL << 63 | Encoded) == Flags.NEGATIVE_INFINITY_BITS;
        public bool IsPositiveInfinity => Encoded == Flags.POSITIVE_INFINITY_BITS;
        public bool IsNegativeInfinity => Encoded == Flags.NEGATIVE_INFINITY_BITS;
        public bool IsInt32 => Tag == Flags.INT32_TAG;
        public bool IsIntegralNumber => IsInt32 || (IsFiniteNumber && Math.Round(asDouble()) == asDouble());
        public bool IsPositiveZero => Encoded == 0 || (IsInt32 && asInt32() == 0);
        public bool IsNegativeZero => Encoded == Flags.NEGATIVE_ZERO_BITS;
        public bool IsNull => Tag == Flags.NULL_TAG;
        public bool IsNullish => (Tag & Flags.IS_NULLISH_EXTRACT_PATTERN) == Flags.IS_NULLISH_PATTERN;
        public bool IsNumber => IsDouble || IsInt32;
        public bool IsNaN => Encoded == Flags.CANON_NAN_BITS;
        public bool IsObject => Tag == Flags.OBJECT_TAG;
        public bool IsString => Tag == Flags.STRING_TAG;
        public bool IsSymbol => Tag == Flags.SYMBOL_TAG;
        public bool IsUndefined => Tag == Flags.UNDEFINED_TAG;

        internal Value(IntPtr ptr)
        {
            m_ptr = ptr;
        }

        public double asDouble()
        {
            if (!IsNumber)
                throw new InvalidOperationException("Value is not a number");
            if (IsInt32)
                return asInt32();
            return js_value_as_double(m_ptr);
        }

        public int asInt32() 
        {
            if (!IsInt32)
                throw new InvalidOperationException("Value is not an Int32");
            return (int)(Encoded & 0xFFFFFFFF);
        }

        public override string ToString()
        {
            StringBuilder builder = new StringBuilder();
            js_value_to_string(m_ptr, builder, 2048);
            return builder.ToString();
        }

        public void Dispose()
        {
            if (m_ptr != IntPtr.Zero)
            {
                js_value_free(m_ptr);
                m_ptr = IntPtr.Zero;
            }
        }
    }
}