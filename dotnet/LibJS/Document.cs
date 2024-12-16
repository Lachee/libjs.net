using System.Diagnostics;
using System.Runtime.InteropServices;

namespace LibJS
{
    public delegate void NativeFunction(Document environment, Array args);
    public class Document : IUnmanagedObject
    {
        internal const string LibraryName = "LibJSNet";
        // [DllImport(LIB)] static extern IntPtr create_environment();
        // [UnmanagedFunctionPointer(CallingConvention.Cdecl)] delegate void ActionPtr();
        // [DllImport(LIB)] static extern void set_invoke(IntPtr environment, IntPtr function);
        // [DllImport(LIB)] static extern void run(IntPtr environment, string source);

        [DllImport(LibraryName)] static extern IntPtr environment_create();
        [DllImport(LibraryName)] static extern IntPtr environmnet_evaluate(IntPtr environment, string source, string source_name);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)] delegate void OnLogCallback(int level, string message);
        [DllImport(LibraryName)] static extern void environment_set_on_console_log(IntPtr enviornment, IntPtr callback);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)] delegate void FunctionCallback(IntPtr args_ptr);
        [DllImport(LibraryName)] static extern void environment_define_function(IntPtr enviornment, string name, IntPtr callback);
        [DllImport(LibraryName)] static extern IntPtr js_value_call(IntPtr environment, IntPtr value);

        private IntPtr m_ptr;
        public IntPtr Ptr => m_ptr;

        public event Action<LogLevel, string>? OnLog;

        public Document()
        {
            m_ptr = environment_create();

            OnLogCallback onLogCallback = new OnLogCallback((level, msg) => OnLog?.Invoke((LogLevel)level, msg));
            environment_set_on_console_log(m_ptr, Marshal.GetFunctionPointerForDelegate(onLogCallback));
        }

        public void DefineFunction(string name, NativeFunction func)
        {
            var action = new FunctionCallback((argPtr) => {
                func(this, new Array(argPtr));
            });
            environment_define_function(m_ptr, name, Marshal.GetFunctionPointerForDelegate(action));
        }

        public Value? Evaluate(string script, string? scriptName = null)
        {
            var ptr = environmnet_evaluate(m_ptr, script, scriptName ?? "Environment.Run");
            if (ptr == IntPtr.Zero)
                throw new System.Exception("Failed to evaluate the script.");

            if (ptr == IntPtr.Zero)
                return null;

            return new Value(ptr);
        }

         /// <summary>
        /// Invokes the value as a function
        /// </summary>
        /// <returns>Result of the function call.</returns>
        /// <exception cref="InvalidOperationException">If the value is not a function</exception>
        public Value? Call(Value function) {
            if (!function.IsFunction)
                throw new InvalidOperationException("Value is not a function");

            IntPtr result_ptr = js_value_call(m_ptr, function.Ptr);
            if (result_ptr == IntPtr.Zero)
                throw new InvalidOperationException("Failed to invoke the function");

            Value value = new Value(result_ptr);
            if (value.IsUndefined || value.IsNull)  {
                value.Dispose();
                return null;
            }
            return value;
        }

        public void Dispose()
        {
            throw new NotImplementedException();
        }
    }
}