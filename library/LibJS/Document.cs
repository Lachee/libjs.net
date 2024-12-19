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

        [DllImport(LibraryName)] static extern IntPtr document_create();
        [DllImport(LibraryName)] static extern void document_load_script(IntPtr document, string source, string source_name);
        
        [DllImport(LibraryName)] static extern ulong document_evaluate(IntPtr document, string source, string source_name);
        
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)] delegate void OnLogCallback(int level, string message);
        [DllImport(LibraryName)] static extern void document_set_on_console_log(IntPtr document, IntPtr callback);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)] delegate void FunctionCallback(IntPtr args_ptr);
        [DllImport(LibraryName)] static extern void document_define_function(IntPtr document, string name, IntPtr callback);
        [DllImport(LibraryName)] static extern void document_call_last_value(IntPtr document);

        private IntPtr m_ptr;
        public IntPtr Ptr => m_ptr;

        public event Action<LogLevel, string>? OnLog;

        public Document()
        {
            m_ptr = document_create();

            OnLogCallback onLogCallback = new OnLogCallback((level, msg) => OnLog?.Invoke((LogLevel)level, msg));
            document_set_on_console_log(m_ptr, Marshal.GetFunctionPointerForDelegate(onLogCallback));
        }

        public void DefineFunction(string name, NativeFunction func)
        {
            var action = new FunctionCallback((argPtr) =>
            {
                func(this, new Array(argPtr));
            });
            document_define_function(m_ptr, name, Marshal.GetFunctionPointerForDelegate(action));
        }

        public Value Evaluate(string script, string? scriptName = null)
        {
            var encoded = document_evaluate(m_ptr, script, scriptName ?? "Environment.Run");
            return new Value(encoded);
        }

        /// <summary>
        /// Invokes the value as a function
        /// </summary>
        /// <returns>Result of the function call.</returns>
        /// <exception cref="InvalidOperationException">If the value is not a function</exception>
        public Value? Call(Value function)
        {
            document_call_last_value(m_ptr);
            return null;
        }

        public void Dispose()
        {
            throw new NotImplementedException();
        }
    }
}