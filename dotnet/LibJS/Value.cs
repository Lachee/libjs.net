using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;


namespace LibJS
{
    public sealed class Value : IUnmanagedObject
    {
        const int STRING_MAX_LENGTH = 2048;

        [DllImport(Document.LibraryName)] static extern void js_value_free(IntPtr value);
        [DllImport(Document.LibraryName)] static extern ushort js_value_tag(IntPtr value);
        [DllImport(Document.LibraryName)] static extern ushort js_value_encoded(IntPtr value);
        [DllImport(Document.LibraryName)] static extern int js_value_to_string(IntPtr value, byte[] strBuff, int strBuffSize);
        [DllImport(Document.LibraryName)] static extern double js_value_as_double(IntPtr value);
        [DllImport(Document.LibraryName)] static extern bool js_value_is_function(IntPtr value);
        [DllImport(Document.LibraryName)] static extern bool js_value_is_constructor(IntPtr value);
        [DllImport(Document.LibraryName)] static extern bool js_value_is_error(IntPtr value);
        [DllImport(Document.LibraryName)] static extern void js_value_invoke(uint value);
        [DllImport(Document.LibraryName)] static extern IntPtr js_value_call(IntPtr environment, IntPtr value);

        public static void Invoke(uint value) 
        {
            js_value_invoke(value);
        }

        private IntPtr m_ptr;
        public IntPtr Ptr => m_ptr;

        private ushort? m_tag;
        private ushort Tag => m_tag ??= js_value_tag(m_ptr);

        private ulong? m_encoded;
        private ulong Encoded => m_encoded ??= js_value_encoded(m_ptr);

        public bool IsAccessor => Tag == Flags.ACCESSOR_TAG;
        public bool IsBigInt => Tag == Flags.BIGINT_TAG;
        public bool IsBoolean => Tag == Flags.BOOLEAN_TAG;
        public bool IsContructor => js_value_is_constructor(m_ptr);
        public bool IsDouble => (Encoded & Flags.CANON_NAN_BITS) != Flags.CANON_NAN_BITS || (Encoded == Flags.CANON_NAN_BITS);
        public bool IsEmpty => Tag == Flags.EMPTY_TAG;
        public bool IsError => js_value_is_error(m_ptr);
        public bool IsFiniteNumber => IsNumber && (IsInt32 || (!IsNaN && !IsInfinity));
        public bool IsFunction => js_value_is_function(m_ptr);
        public bool IsInfinity => (0x1UL << 63 | Encoded) == Flags.NEGATIVE_INFINITY_BITS;
        public bool IsPositiveInfinity => Encoded == Flags.POSITIVE_INFINITY_BITS;
        public bool IsNegativeInfinity => Encoded == Flags.NEGATIVE_INFINITY_BITS;
        public bool IsInt32 => Tag == Flags.INT32_TAG;
        public bool IsIntegralNumber => IsInt32 || (IsFiniteNumber && Math.Round(AsDouble()) == AsDouble());
        public bool IsPositiveZero => Encoded == 0 || (IsInt32 && AsInt32() == 0);
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
            Debug.Assert(ptr != IntPtr.Zero);
            m_ptr = ptr;
        }

        public double AsDouble()
        {
            if (!IsNumber)
                throw new InvalidOperationException("Value is not a number");
            if (IsInt32)
                return AsInt32();
            return js_value_as_double(m_ptr);
        }

        public int AsInt32() 
        {
            if (!IsInt32)
                throw new InvalidOperationException("Value is not an Int32");
            return (int)(Encoded & 0xFFFFFFFF);
        }

        public string AsString() 
        {
            if (m_ptr == IntPtr.Zero)
                return "<nullptr>";

            byte[] buffer = new byte[STRING_MAX_LENGTH];
            int size = js_value_to_string(m_ptr, buffer, buffer.Length);
            return Encoding.UTF8.GetString(buffer, 0, size);
        }

        // /// <summary>
        // /// Invokes the value as a function
        // /// </summary>
        // /// <returns>Result of the function call.</returns>
        // /// <exception cref="InvalidOperationException">If the value is not a function</exception>
        // public Value? Invoke() {
        //     if (!IsFunction)
        //         throw new InvalidOperationException("Value is not a function");

        //     IntPtr result_ptr = js_value_call(m_ptr);
        //     if (result_ptr == IntPtr.Zero)
        //         throw new InvalidOperationException("Failed to invoke the function");

        //     Value value = new Value(result_ptr);
        //     if (value.IsUndefined || value.IsNull)  {
        //         value.Dispose();
        //         return null;
        //     }
        //     return value;
        // }

        public override string ToString()
        {
            return AsString();
        }

        public void Dispose()
        {
            return;

            if (m_ptr != IntPtr.Zero)
            {
                js_value_free(m_ptr);
                m_ptr = IntPtr.Zero;
            }
        }
    }
}