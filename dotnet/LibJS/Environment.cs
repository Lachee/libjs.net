using System.Diagnostics;
using System.Runtime.InteropServices;

namespace LibJS
{
    public enum LogLevel {
        Assert = 0,
        Count,
        CountReset,
        Debug,
        Dir,
        DirXML,
        Error,
        Group,
        GroupCollapsed,
        Info,
        Log,
        TimeEnd,
        TimeLog,
        Table,
        Trace,
        Warn,
    };

    public delegate void NativeFunction(Environment environment, Array args);
    public class Environment
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

        private IntPtr m_ptr;

        public event Action<LogLevel, string>? OnLog;

        public Environment()
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

        public Value Evaluate(string script, string? scriptName = null)
        {
            var ptr = environmnet_evaluate(m_ptr, script, scriptName ?? "Environment.Run");
            if (ptr == IntPtr.Zero)
                throw new System.Exception("Failed to evaluate the script.");

            return new Value(ptr);
        }
    }
}