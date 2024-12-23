using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using static System.Runtime.InteropServices.JavaScript.JSType;

namespace LibJS.Types
{
	public class Function : Object
	{
		[DllImport(Consts.LibraryName)] static extern Value js_function_invoke(Value value);

		public Function(ulong value) : base(value) { }
		public Function(Value value) : base(value) {
			if (!value.IsFunction)
				throw new InvalidTypeException("function");
		}

		/// <summary>Invokes the value as a function</summary>
		public Value Invoke()
		{
			return js_function_invoke(value);
			//return new Value(result);
		}
	}
}
