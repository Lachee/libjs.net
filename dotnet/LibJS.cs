using System.Runtime.InteropServices;

class LibJS
{
    private static class Extern
    {
        const string DLL_NAME = "LibJSNet";

		[DllImport(DLL_NAME)]
		public static extern int run_script(string str);

        [DllImport(DLL_NAME)]
		public static extern void register_function(string name, IntPtr ptr);
	}

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
	public delegate void PInvokeCallback();

	public static int RunScript(string str) {
        return Extern.run_script(str);
    }

    public static void RegisterFunction(string name, PInvokeCallback function)
    {
        IntPtr ptr = Marshal.GetFunctionPointerForDelegate(function);
        Extern.register_function(name, ptr);
    }
}

