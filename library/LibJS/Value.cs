using System.Diagnostics;
using System.Diagnostics.CodeAnalysis;
using System.Runtime.InteropServices;
using System.Text;


namespace LibJS
{
	[StructLayout(LayoutKind.Explicit)]
	public struct Value 
    {
        const int STRING_MAX_LENGTH = 2048;
		[DllImport(Document.LibraryName)] static extern int js_value_to_string(ulong value, byte[] strBuff, int strBuffSize);
		[DllImport(Document.LibraryName)] static extern bool js_value_is_function(ulong value);
		[DllImport(Document.LibraryName)] static extern bool js_value_is_constructor(ulong value);
		[DllImport(Document.LibraryName)] static extern bool js_value_is_error(ulong value);

		[FieldOffset(0)]
		private double m_double;

		[FieldOffset(0)]
		private ulong m_encoded;

		public ulong Payload
		{
			get => m_encoded & 0xFFFFFFFFFFFF; // Mask for 48 bits
			set => m_encoded = (m_encoded & ~0xFFFFFFFFFFFFUL) | (value & 0xFFFFFFFFFFFF);
		}
		public ulong Tag
		{
			get => (m_encoded >> 48) & 0xFFFF; // Shift and mask for 16 bits
			set => m_encoded = (m_encoded & ~(0xFFFFUL << 48)) | ((value & 0xFFFF) << 48);
		}

		public bool IsObject => Tag == Flags.OBJECT_TAG;
		public bool IsBoolean => Tag == Flags.BOOLEAN_TAG;
		public bool IsAccessor => Tag == Flags.ACCESSOR_TAG;
		public bool IsBigInt => Tag == Flags.BIGINT_TAG;
		public bool IsEmpty => Tag == Flags.EMPTY_TAG;
		public bool IsInt32 => Tag == Flags.INT32_TAG;
		public bool IsUndefined => Tag == Flags.UNDEFINED_TAG;
		public bool IsNull => Tag == Flags.NULL_TAG;
		public bool IsNullish => (Tag & Flags.IS_NULLISH_EXTRACT_PATTERN) == Flags.IS_NULLISH_PATTERN;
		public bool IsString => Tag == Flags.STRING_TAG;
		public bool IsSymbol => Tag == Flags.SYMBOL_TAG;

		public bool IsDouble => (m_encoded & Flags.CANON_NAN_BITS) != Flags.CANON_NAN_BITS || (m_encoded == Flags.CANON_NAN_BITS);
		public bool IsFiniteNumber => IsNumber && (IsInt32 || (!IsNaN && !IsInfinity));
		public bool IsInfinity => (0x1UL << 63 | m_encoded) == Flags.NEGATIVE_INFINITY_BITS;
		public bool IsPositiveInfinity => m_encoded == Flags.POSITIVE_INFINITY_BITS;
		public bool IsNegativeInfinity => m_encoded == Flags.NEGATIVE_INFINITY_BITS;
		public bool IsIntegralNumber => IsInt32 || (IsFiniteNumber && Math.Round(AsDouble()) == AsDouble());
		public bool IsPositiveZero => m_encoded == 0 || (IsInt32 && AsInt() == 0);
		public bool IsNegativeZero => m_encoded == Flags.NEGATIVE_ZERO_BITS;
		public bool IsNumber => IsDouble || IsInt32;
		public bool IsNaN => m_encoded == Flags.CANON_NAN_BITS;

		public bool IsFunction => IsObject && js_value_is_function(m_encoded);
		public bool IsContructor => IsObject && js_value_is_constructor(m_encoded);
		public bool IsError => IsObject && js_value_is_error(m_encoded);

		/// <summary>Gets the value as a double</summary>
		/// <returns></returns>
		public double AsDouble() {
			Debug.Assert(IsNumber);
			if (IsInt32)
				return (double)AsInt();
			return this.m_double;
		}

		/// <summary>Gets the value as a integer</summary>
		/// <returns></returns>
		public int AsInt() {
			Debug.Assert(IsInt32);
			return (int)(m_encoded & 0xFFFFFFFF);
		}

		public bool AsBoolean() {
			Debug.Assert(IsBoolean);
			return (m_encoded & 0x1) != 0;
		}

		public string AsString() {
			//Debug.Assert(IsString);
			byte[] buffer = new byte[STRING_MAX_LENGTH];
			int length = js_value_to_string(m_encoded, buffer, buffer.Length);
			return Encoding.UTF8.GetString(buffer, 0, length);
		}

		internal Value(ulong encoded)
		{
			m_double = 0;
			m_encoded = encoded;
		}

		internal Value(double value)
		{
			m_encoded = 0;
			m_double = value;
		}

		public override string ToString()
        {
			return AsString();

			if (IsInt32)
				return AsInt().ToString();
			if (IsNumber)
				return AsDouble().ToString();
			return AsString();
        }

		public override int GetHashCode()
		{
			return m_encoded.GetHashCode();
		}

		public override bool Equals([NotNullWhen(true)] object? obj)
		{
			if (obj is ulong u64)
				return this.Equals(new Value(u64));
			if (obj is double dbl)
				return this.Equals(new Value(dbl));
			
			if (obj is Value rhs) 
			{
				// 1. If Type(x) is different from Type(y), return false.
				if (!SameTypeForEquality(this, rhs))
					return false;

				// 2. If x is a Number, then
				if (this.IsNumber)
				{
					// a. Return Number::sameValue(x, y).

					// 6.1.6.1.14 Number::sameValue ( x, y ), https://tc39.es/ecma262/#sec-numeric-types-number-sameValue
					// 1. If x is NaN and y is NaN, return true.
					if (this.IsNaN && rhs.IsNaN)
						return true;
					// 2. If x is +0𝔽 and y is -0𝔽, return false.
					if (this.IsPositiveZero && rhs.IsNegativeZero)
						return false;
					// 3. If x is -0𝔽 and y is +0𝔽, return false.
					if (this.IsNegativeZero && rhs.IsPositiveZero)
						return false;
					// 4. If x is the same Number value as y, return true.
					// 5. Return false.
					return this.AsDouble() == rhs.AsDouble();
				}

				// 3. Return SameValueNonNumber(x, y).
				return SameValueNonNumber(this, rhs);
			}
			
			return false;
		}

		public static bool operator ==(Value lhs, Value rhs) => lhs.Equals(rhs);
		public static bool operator !=(Value lhs, Value rhs) => !lhs.Equals(rhs);

		private static bool SameTypeForEquality(Value lhs, Value rhs) {
			if ((lhs.m_encoded & Flags.TAG_EXTRACTION) == (rhs.m_encoded & Flags.TAG_EXTRACTION))
				return true;

			if (lhs.IsNumber && rhs.IsNumber)
				return true;

			return false;
		}

		private static bool SameValueNonNumber(Value lhs, Value rhs) {
			// 1. Assert: Type(x) is the same as Type(y).
			Debug.Assert(SameTypeForEquality(lhs, rhs));
			Debug.Assert(!lhs.IsNumber);

			// 2. If x is a BigInt, then
			if (lhs.IsBigInt)
			{
				// a. Return BigInt::equal(x, y).

				// 6.1.6.2.13 BigInt::equal ( x, y ), https://tc39.es/ecma262/#sec-numeric-types-bigint-equal
				// 1. If ℝ(x) = ℝ(y), return true; otherwise return false.

				// FIXME: Implement this
				//return lhs.as_bigint().big_integer() == rhs.as_bigint().big_integer();
			}

			// 5. If x is a String, then
			if (lhs.IsString)
			{
				// a. If x and y are exactly the same sequence of code units (same length and same code units at corresponding indices), return true; otherwise, return false.
				return lhs.AsString().Equals(rhs.AsString());
			}

			// 3. If x is undefined, return true.
			// 4. If x is null, return true.
			// 6. If x is a Boolean, then
			//    a. If x and y are both true or both false, return true; otherwise, return false.
			// 7. If x is a Symbol, then
			//    a. If x and y are both the same Symbol value, return true; otherwise, return false.
			// 8. If x and y are the same Object value, return true. Otherwise, return false.
			// NOTE: All the options above will have the exact same bit representation in Value, so we can directly compare the bits.
			return lhs.m_encoded == rhs.m_encoded;
		}
	}
}