using System.Diagnostics;
using System.Runtime.InteropServices;

namespace LibJS
{


    public class Environment
    {
        const string LIB = "LibJSNet";
        // [DllImport(LIB)] static extern IntPtr create_environment();
        // [UnmanagedFunctionPointer(CallingConvention.Cdecl)] delegate void ActionPtr();
        // [DllImport(LIB)] static extern void set_invoke(IntPtr environment, IntPtr function);
        // [DllImport(LIB)] static extern void run(IntPtr environment, string source);

        [DllImport(LIB)] static extern IntPtr extern_create_environment();
        [DllImport(LIB)] static extern bool extern_parse_and_run(IntPtr environment, string source, string source_name);

        private IntPtr _environment;

        public event Action? OnInvoke;

        public Environment()
        {
            _environment = extern_create_environment();
            // ActionPtr externInvokeCallback = new ActionPtr(() => { OnInvoke?.Invoke(); });
            // set_invoke(_environment, Marshal.GetFunctionPointerForDelegate(externInvokeCallback));
        }

        public void Run(string source, string? sourceName = null)
        {
            extern_parse_and_run(_environment, source, sourceName ?? "Environment.Run(string, string)");
        }
    }
}