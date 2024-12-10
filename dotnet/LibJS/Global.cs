using System.Runtime.InteropServices;

namespace LibJS
{
    class Global
    {
        private static class Extern
        {
            const string DLL_NAME = "LibJSNet";

            [DllImport(DLL_NAME)]
            public static extern int run_global_script(string str);

            [DllImport(DLL_NAME)]
            public static extern void register_global_function(string name, IntPtr ptr);
        }

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void PInvokeCallback();

        public static int RunScript(string str)
        {
            return Extern.run_global_script(str);
        }

        public static void RegisterFunction(string name, PInvokeCallback function)
        {
            IntPtr ptr = Marshal.GetFunctionPointerForDelegate(function);
            Extern.register_global_function(name, ptr);
        }
    }
}