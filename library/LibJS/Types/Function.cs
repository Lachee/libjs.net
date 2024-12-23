using System.Runtime.InteropServices;

namespace LibJS.Types
{
	public class Function : Object
	{
		[DllImport(Consts.LibraryName)] static extern Value js_function_invoke(Value value, Value[] arguments, int length);

		public Function(ulong value) : base(value) { }
		public Function(Value value) : base(value) {
			if (!value.IsFunction)
				throw new InvalidTypeException("function");
		}

		/// <summary>Invokes the value as a function</summary>
		public Value Invoke(params Value[] args)
		{
			return js_function_invoke(value, args, args.Length);
		}
	}
}
