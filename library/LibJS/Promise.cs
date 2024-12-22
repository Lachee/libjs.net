using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace LibJS
{
	public class Promise : Object
	{
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)] delegate void PromiseCallback(ulong value);
		[DllImport(Consts.LibraryName)] private static extern void js_object_then(ulong value, IntPtr resolve);
		internal Promise(Value value) : base(value)
		{
			Debug.Assert(value.IsPromise);
		}

		public Task<Value> Then() 
		{
			var tcs = new TaskCompletionSource<Value>();
			PromiseCallback callback = new PromiseCallback((value) => tcs.SetResult(new Value(value)));
			js_object_then(Value.Encoded, Marshal.GetFunctionPointerForDelegate(callback));
			return tcs.Task;
		}
	}
}
