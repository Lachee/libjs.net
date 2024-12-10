using System.Runtime.InteropServices;

namespace LibJS
{


    public class Environment
    {
        const string LIB = "LibJSNet";
        [DllImport(LIB)] static extern IntPtr create_environment();
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)] delegate void ActionPtr();
        [DllImport(LIB)] static extern void set_invoke(IntPtr environment, IntPtr function);
        [DllImport(LIB)] static extern void run(IntPtr environment, string source);

        private IntPtr _environment;

        public event Action? OnInvoke;

        public Environment()
        {
            _environment = create_environment();
            ActionPtr externInvokeCallback = new ActionPtr(() => { OnInvoke?.Invoke(); });
            set_invoke(_environment, Marshal.GetFunctionPointerForDelegate(externInvokeCallback));
        }
        
        public void Run(string source) {
            run(_environment, source);
        }
    }
}