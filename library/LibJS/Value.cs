using System.Diagnostics;
using System.Diagnostics.CodeAnalysis;
using System.Runtime.InteropServices;
using System.Text;


namespace LibJS
{
	[StructLayout(LayoutKind.Explicit)]
	public struct Value
	{
		[DllImport(Consts.LibraryName)] static extern int js_value_to_string(ulong value, byte[] strBuff, int strBuffSize);
		[DllImport(Consts.LibraryName)] static extern bool js_value_is_function(ulong value);
		[DllImport(Consts.LibraryName)] static extern bool js_value_is_constructor(ulong value);
		[DllImport(Consts.LibraryName)] static extern bool js_value_is_error(ulong value);
		[DllImport(Consts.LibraryName)] static extern bool js_value_is_array(ulong value);
		[DllImport(Consts.LibraryName)] static extern bool js_value_is_regexp(ulong value);
		[DllImport(Consts.LibraryName)] static extern ulong js_value_invoke(ulong value);

		public static Value Undefined => new Value(Tags.UNDEFINED_TAG, 0UL);
		public static Value Null => new Value(Tags.NULL_TAG, 0UL);
		public static Value NaN => new Value((-(float)(((float)(1e+300 * 1e+300)) * 0.0F)));
		public static Value Infinity => new Value(((float)(1e+300 * 1e+300)));
		public static Value NegativeInfinity => new Value(-((float)(1e+300 * 1e+300)));

		[FieldOffset(0)]
		private double m_double;

		[FieldOffset(0)]
		private ulong m_encoded;
		public ulong Encoded => m_encoded;

		private ulong Payload
		{
			get => m_encoded & 0xFFFFFFFFFFFF; // Mask for 48 bits
			set => m_encoded = (m_encoded & ~0xFFFFFFFFFFFFUL) | (value & 0xFFFFFFFFFFFF);
		}
		private ulong Tag
		{
			get => (m_encoded >> Tags.TAG_SHIFT) & 0xFFFF; // Shift and mask for 16 bits
			set => m_encoded = (m_encoded & ~(0xFFFFUL << Tags.TAG_SHIFT)) | ((value & 0xFFFF) << Tags.TAG_SHIFT);
		}

		public bool IsObject => Tag == Tags.OBJECT_TAG;
		public bool IsBoolean => Tag == Tags.BOOLEAN_TAG;
		public bool IsAccessor => Tag == Tags.ACCESSOR_TAG;
		public bool IsBigInt => Tag == Tags.BIGINT_TAG;
		public bool IsEmpty => Tag == Tags.EMPTY_TAG;
		public bool IsInt32 => Tag == Tags.INT32_TAG;
		public bool IsUndefined => Tag == Tags.UNDEFINED_TAG;
		public bool IsNull => Tag == Tags.NULL_TAG;
		public bool IsNullish => (Tag & Tags.IS_NULLISH_EXTRACT_PATTERN) == Tags.IS_NULLISH_PATTERN;
		public bool IsString => Tag == Tags.STRING_TAG;
		public bool IsSymbol => Tag == Tags.SYMBOL_TAG;

		public bool IsDouble => (m_encoded & Tags.CANON_NAN_BITS) != Tags.CANON_NAN_BITS || (m_encoded == Tags.CANON_NAN_BITS);
		public bool IsFiniteNumber => IsNumber && (IsInt32 || (!IsNaN && !IsInfinity));
		public bool IsInfinity => (0x1UL << 63 | m_encoded) == Tags.NEGATIVE_INFINITY_BITS;
		public bool IsPositiveInfinity => m_encoded == Tags.POSITIVE_INFINITY_BITS;
		public bool IsNegativeInfinity => m_encoded == Tags.NEGATIVE_INFINITY_BITS;
		public bool IsIntegralNumber => IsInt32 || (IsFiniteNumber && Math.Round(AsDouble()) == AsDouble());
		public bool IsPositiveZero => m_encoded == 0 || (IsInt32 && AsInt() == 0);
		public bool IsNegativeZero => m_encoded == Tags.NEGATIVE_ZERO_BITS;
		public bool IsNumber => IsDouble || IsInt32;
		public bool IsNaN => m_encoded == Tags.CANON_NAN_BITS;

		public bool IsFunction => IsObject && js_value_is_function(m_encoded);
		public bool IsContructor => IsObject && js_value_is_constructor(m_encoded);
		public bool IsError => IsObject && js_value_is_error(m_encoded);
		public bool IsArray => IsObject && js_value_is_array(m_encoded);
		public bool IsRegexp => IsObject && js_value_is_regexp(m_encoded);

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

		internal Value(ulong tag, ulong payload) 
			: this((tag << Tags.TAG_SHIFT) | payload)
		{
		}

		/// <summary>Gets the value as a double</summary>
		/// <returns></returns>
		public double AsDouble()
		{
			Debug.Assert(IsNumber);
			if (IsInt32)
				return (double)AsInt();
			return this.m_double;
		}

		/// <summary>Gets the value as a integer</summary>
		/// <returns></returns>
		public int AsInt()
		{
			Debug.Assert(IsInt32);
			return (int)(m_encoded & 0xFFFFFFFF);
		}

		/// <summary>Gets the value as a boolean</summary>
		/// <returns></returns>
		public bool AsBoolean()
		{
			Debug.Assert(IsBoolean);
			return (m_encoded & 0x1) != 0;
		}

		/// <summary>Gets the underlying pointer of the object</summary>
		/// <returns></returns>
		internal UIntPtr AsPtr() 
		{
			Debug.Assert(IsObject);
			if (System.Environment.Is64BitProcess) 
			{
				return new UIntPtr((m_encoded << 16) >> 16);	
			} 
			else 
			{
				return new UIntPtr(m_encoded & 0xffff_ffff);
			}
		}

		/// <summary>Invokes the value as a function</summary>
		public Value Invoke()
		{
			if (!IsFunction)
				throw new InvalidOperationException("Value must be a function");
			ulong result = js_value_invoke(m_encoded);
			return new Value(result);
		}

		public static Value Create(double value) => new Value(value);
        public static Value Create(int value) => new Value() {
			Tag = Tags.INT32_TAG,
			Payload = (ulong)value
		};

        public override string ToString()
		{
			if (IsInt32)
				return AsInt().ToString();
			if (IsNumber)
				return AsDouble().ToString();
			if (IsBoolean)
				return AsBoolean().ToString();

			byte[] buffer = new byte[Consts.StringMarshalLength];
			int length = js_value_to_string(m_encoded, buffer, buffer.Length);
			return Encoding.UTF8.GetString(buffer, 0, length);
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

		private static bool SameTypeForEquality(Value lhs, Value rhs)
		{
			if ((lhs.m_encoded & Tags.TAG_EXTRACTION) == (rhs.m_encoded & Tags.TAG_EXTRACTION))
				return true;

			if (lhs.IsNumber && rhs.IsNumber)
				return true;

			return false;
		}

		private static bool SameValueNonNumber(Value lhs, Value rhs)
		{
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
				return lhs.ToString().Equals(rhs.ToString());
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