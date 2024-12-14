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

    public class Environment
    {
        internal const string LibraryName = "LibJSNet";
        // [DllImport(LIB)] static extern IntPtr create_environment();
        // [UnmanagedFunctionPointer(CallingConvention.Cdecl)] delegate void ActionPtr();
        // [DllImport(LIB)] static extern void set_invoke(IntPtr environment, IntPtr function);
        // [DllImport(LIB)] static extern void run(IntPtr environment, string source);

        [DllImport(LibraryName)] static extern IntPtr e_environment_create();
        [DllImport(LibraryName)] static extern IntPtr e_environment_evaluate(IntPtr environment, string source, string source_name);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)] delegate void OnLogCallback(int level, string message);
        [DllImport(LibraryName)] static extern void e_environment_set_on_console_log(IntPtr enviornment, IntPtr callback);

        private IntPtr m_ptr;

        public event Action<LogLevel, string>? OnLog;

        public Environment()
        {
            m_ptr = e_environment_create();

            OnLogCallback onLogCallback = new OnLogCallback((level, msg) => OnLog?.Invoke((LogLevel)level, msg));
            e_environment_set_on_console_log(m_ptr, Marshal.GetFunctionPointerForDelegate(onLogCallback));
        }

        public Value Evaluate(string source, string? sourceName = null)
        {
            var ptr = e_environment_evaluate(m_ptr, source, sourceName ?? "Environment.Run(string, string)");
            return new Value(ptr);
        }
    }
}