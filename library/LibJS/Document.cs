using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;

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

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)] delegate void OnLogCallback(int level, IntPtr buff, int buffSize);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)] delegate ulong FunctionCallback(ulong args);

		[DllImport(Consts.LibraryName)] static extern UIntPtr document_create();
        [DllImport(Consts.LibraryName)] static extern ulong document_evaluate(UIntPtr document, string source, string source_name);
        [DllImport(Consts.LibraryName)] static extern void document_set_on_console_log(UIntPtr document, IntPtr callback);
        [DllImport(Consts.LibraryName)] static extern void document_define_function(UIntPtr document, string name, IntPtr callback);

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
                catch(Exception e)
                {
					return Object.Create(e).Value.Encoded;
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
				catch (Exception e)
				{
                    return Object.Create(e).Value.Encoded;
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
            throw new NotSupportedException("Document cannot be disposed");
        }
    }
}