namespace LibJS
{
    internal static class Consts 
    {
        public const string LibraryName = "LibJSNet";
        public const int StringMarshalLength = 2048;
    }

    /// <summary>
    /// Tags for the GC and JS values
    /// </summary>
    /// <remarks>Based of the values within Ladybird</remarks>
    internal static class Tags
    {
        public const int TAG_SHIFT = 48;
        public const ulong CANON_NAN_BITS = 0x7FF8000000000000;
        public const ulong POSITIVE_INFINITY_BITS = 0x7FF0000000000000;
        public const ulong NEGATIVE_INFINITY_BITS = 0xFFF0000000000000;
        public const ulong NEGATIVE_ZERO_BITS = 1UL << 63;
        public const ulong BASE_TAG = 0x7FF8; 
        public const ulong TAG_EXTRACTION = 0xFFFF000000000000;
        public const ulong IS_CELL_BIT = 0x8000 | BASE_TAG;
        public const ulong OBJECT_TAG = 0b001 | IS_CELL_BIT;
        public const ulong STRING_TAG = 0b010 | IS_CELL_BIT;
        public const ulong SYMBOL_TAG = 0b011 | IS_CELL_BIT;
        public const ulong ACCESSOR_TAG = 0b100 | IS_CELL_BIT;
        public const ulong BIGINT_TAG = 0b101 | IS_CELL_BIT;
        public const ulong UNDEFINED_TAG = 0b110 | BASE_TAG;
        public const ulong NULL_TAG = 0b111 | BASE_TAG;
        public const ulong BOOLEAN_TAG = 0b001 | BASE_TAG;
        public const ulong INT32_TAG = 0b010 | BASE_TAG;
        public const ulong EMPTY_TAG = 0b011 | BASE_TAG;
        public const ulong IS_NULLISH_EXTRACT_PATTERN = 0xFFFEU;
        public const ulong IS_NULLISH_PATTERN = 0x7FFEU;
    }
}