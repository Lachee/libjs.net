using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;

namespace LibJS.Types
{
    public class Object
    {
        [DllImport(Consts.LibraryName)] protected static extern ulong js_object_get_property_value_at_index(ulong value, int index);
        [DllImport(Consts.LibraryName)] protected static extern ulong js_object_get_property_value(ulong value, string name);
        [DllImport(Consts.LibraryName)] private static extern ulong js_object_create_error(string message, string stackTrace);

        protected Value value;
        public Value Value => value;

        protected internal nuint Ptr
        {
            get
            {
                if (Environment.Is64BitProcess)
                    return new nuint(value.Encoded << 16 >> 16);
                return new nuint(value.Encoded & 0xffff_ffff);
            }
        }

        internal Object(ulong encoded) : this(new Value(encoded)) { }
        internal Object(Value value)
        {
            if (!value.IsObject)
                throw new InvalidTypeException("object");
            this.value = value;
        }

        [Obsolete("Use Value to wrap pointers")]
        internal Object(nuint ptr)
            : this(new Value(Tags.OBJECT_TAG, ptr.ToUInt64())) { }

        public Value GetProperty(int index)
        {
            var property = js_object_get_property_value_at_index(this.value.Encoded, index);
            var value = new Value(property);
            if (value.IsUndefined)
                throw new IndexUndefinedException(index);

            return value;
        }

        public Value GetProperty(string name)
        {
            var property = js_object_get_property_value(this.value.Encoded, name);
            var value = new Value(property);
            if (value.IsUndefined)
                throw new PropertyUndefinedException(name);

            return value;
        }

        public static Object Create(Exception exception)
        {
            var message = $"{exception.GetType().FullName}: {exception.Message}";
            var stackTrace = "";

            // Convert the trace to be more like javascript's
            if (exception.StackTrace != null)
                stackTrace = StackTraceToJS(exception.StackTrace, true).TrimEnd();

            var value = new Value(js_object_create_error(message, stackTrace.ToString()));
            return new Object(value);
        }

        private static string StackTraceToJS(string stackTrace, bool formatted)
        {
            // Convert the trace to be more like javascript's
            StringBuilder sb = new StringBuilder();
            var traceLines = stackTrace.Split('\n');
            foreach (var line in traceLines)
            {
                int inIndex = line.IndexOf(" in ");
                if (inIndex < 0)
                {
                    sb.AppendLine(line);
                    continue;
                }

                string segment = line.Substring(inIndex + 4).Trim();
                int segIndex = segment.IndexOf(":line ");
                string filename = segment.Substring(0, segIndex);
                string lineNumber = segment.Substring(segIndex + 6);
                if (formatted)
                {
                    sb.Append(' ', 4).Append("at ").Append(filename).Append(':').AppendLine(lineNumber);
                }
                else
                {
                    sb.Append(filename).Append(':').AppendLine(lineNumber);
                }
            }
            return sb.ToString();
        }
    }
}