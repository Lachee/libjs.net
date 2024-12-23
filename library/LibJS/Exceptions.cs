using System;

namespace LibJS {
    public sealed class PropertyUndefinedException : Exception {
        public PropertyUndefinedException(string name) : base($"Property '{name}' is undefined") {
        }
    }

    public sealed class IndexUndefinedException : Exception {
        public IndexUndefinedException(int index) : base($"Index '{index}' is undefined") {
        }
    }

    public sealed class InvalidTypeException : Exception {
        public InvalidTypeException() : base("Cannot perform operation on this type of JS Value.") { }
        public InvalidTypeException(string expected) : base($"Cannot perform operation on this type of JS Value. Expected a {expected}.") { }
	}
}