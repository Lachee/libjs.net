using System.Runtime.InteropServices;
using System.Text;
using LibJS.Types;

namespace LibJS
{
    public delegate Value NativeFunc(Document document, Array arguments);
    public delegate void NativeAction(Document document, Array arguments);
    public class Document : Object
    {
        // [DllImport(LIB)] static extern UIntPtr create_environment();
        // [UnmanagedFunctionPointer(CallingConvention.Cdecl)] delegate void ActionPtr();
        // [DllImport(LIB)] static extern void set_invoke(UIntPtr environment, UIntPtr function);
        // [DllImport(LIB)] static extern void run(UIntPtr environment, string source);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)] delegate void OnLogCallback(int level, nint buff, int buffSize);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)] delegate ulong FunctionCallback(ulong args);

        [DllImport(Consts.LibraryName)] static extern nuint document_create();
        [DllImport(Consts.LibraryName)] static extern ulong document_evaluate(nuint document, string source, string source_name);
        [DllImport(Consts.LibraryName)] static extern void document_set_on_console_log(nuint document, nint callback);
        [DllImport(Consts.LibraryName)] static extern void document_define_function(nuint document, string name, nint callback);

        public event System.Action<LogLevel, string>? OnLog;

        public Document()
            : base(document_create())
        {
            OnLogCallback onLogCallback = new OnLogCallback((level, buff, buffSize) =>
            {
                byte[] buffer = new byte[buffSize];
                Marshal.Copy(buff, buffer, 0, buffSize);
                OnLog?.Invoke((LogLevel)level, Encoding.UTF8.GetString(buffer));
            });

            document_set_on_console_log(Ptr, Marshal.GetFunctionPointerForDelegate(onLogCallback));
        }


        public void DefineFunction(string name, NativeAction func)
        {
            var action = new FunctionCallback((args) =>
            {
                try
                {
                    func(this, new Array(new Value(args)));
                    return Value.Undefined.Encoded;
                }
                catch (System.Exception e)
                {
                    return CreateError(e).Value.Encoded;
                }
            });
            document_define_function(Ptr, name, Marshal.GetFunctionPointerForDelegate(action));
        }

        public void DefineFunction(string name, NativeFunc func)
        {
            var action = new FunctionCallback((args) =>
            {
                try
                {
                    var value = func(this, new Array(new Value(args)));
                    return value.Encoded;
                }
                catch (System.Exception e)
                {
                    return CreateError(e).Value.Encoded;
                }
            });
            document_define_function(Ptr, name, Marshal.GetFunctionPointerForDelegate(action));
        }

        public Value Evaluate(string script, string? scriptName = null)
        {
            var encoded = document_evaluate(Ptr, script, scriptName ?? "Environment.Run");
            return new Value(encoded);
        }

        public void Dispose()
        {
            throw new System.NotSupportedException("Document cannot be disposed");
        }
    }
}