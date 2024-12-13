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
        const string LIB = "LibJSNet";
        // [DllImport(LIB)] static extern IntPtr create_environment();
        // [UnmanagedFunctionPointer(CallingConvention.Cdecl)] delegate void ActionPtr();
        // [DllImport(LIB)] static extern void set_invoke(IntPtr environment, IntPtr function);
        // [DllImport(LIB)] static extern void run(IntPtr environment, string source);

        [DllImport(LIB)] static extern IntPtr extern_create_environment();
        [DllImport(LIB)] static extern bool extern_parse_and_run(IntPtr environment, string source, string source_name);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)] delegate void OnLogCallback(int level, string message);
        [DllImport(LIB)] static extern void extern_set_on_console_log(IntPtr enviornment, IntPtr callback);

        private IntPtr _environment;

        public event Action<LogLevel, string>? OnLog;

        public Environment()
        {
            _environment = extern_create_environment();

            OnLogCallback onLogCallback = new OnLogCallback((level, msg) => OnLog?.Invoke((LogLevel)level, msg));
            extern_set_on_console_log(_environment, Marshal.GetFunctionPointerForDelegate(onLogCallback));
        }

        public void Run(string source, string? sourceName = null)
        {
            extern_parse_and_run(_environment, source, sourceName ?? "Environment.Run(string, string)");
        }
    }
}