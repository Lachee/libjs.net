using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;

namespace LibJS
{
    public delegate void NativeFunction(Document environment, Array args);
    public class Document : Object
    {
        // [DllImport(LIB)] static extern IntPtr create_environment();
        // [UnmanagedFunctionPointer(CallingConvention.Cdecl)] delegate void ActionPtr();
        // [DllImport(LIB)] static extern void set_invoke(IntPtr environment, IntPtr function);
        // [DllImport(LIB)] static extern void run(IntPtr environment, string source);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)] delegate void OnLogCallback(int level, IntPtr buff, int buffSize);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)] delegate void FunctionCallback(IntPtr args_ptr);

        [DllImport(Consts.LibraryName)] static extern IntPtr document_create();
        [DllImport(Consts.LibraryName)] static extern ulong document_evaluate(IntPtr document, string source, string source_name);
        [DllImport(Consts.LibraryName)] static extern void document_set_on_console_log(IntPtr document, IntPtr callback);
        [DllImport(Consts.LibraryName)] static extern void document_define_function(IntPtr document, string name, IntPtr callback);

        public event Action<LogLevel, string>? OnLog;

        public Document()
            : base(document_create())
        {
            OnLogCallback onLogCallback = new OnLogCallback((level, buff, buffSize) =>
            {
                byte[] buffer = new byte[buffSize];
                Marshal.Copy(buff, buffer, 0, buffSize);
                OnLog?.Invoke((LogLevel)level, Encoding.UTF8.GetString(buffer));
            });

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

        public override void Dispose()
        {
            throw new NotSupportedException("Document cannot be disposed");
        }
    }
}