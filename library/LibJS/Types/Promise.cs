using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace LibJS.Types
{
    public class Promise : Object
    {
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)] delegate void PromiseCallback(ulong value);
        [DllImport(Consts.LibraryName)] private static extern void js_object_promise_invoke_on_complete(ulong value, nint then);
        [DllImport(Consts.LibraryName)] private static extern ulong js_object_promise_create(out ulong resolve);
        [DllImport(Consts.LibraryName)] private static extern void run_queued_promise_jobs();

        internal Promise(ulong value) : base(value) { }
        public Promise(Value value) : base(value)
        {
            if (!value.IsPromise)
                throw new InvalidTypeException("promise");
        }

        public Task<Value> AsTask()
        {
            var tcs = new TaskCompletionSource<Value>();
            PromiseCallback callback = new PromiseCallback((value) =>
            {
                Console.WriteLine("Resolved: {0}", value);
                tcs.SetResult(new Value(value));
            });
            js_object_promise_invoke_on_complete(Value.Encoded, Marshal.GetFunctionPointerForDelegate(callback));
            return tcs.Task;
        }

        public static Promise Create(Task task)
        {
            var encPromise = js_object_promise_create(out var encResolveFunc);
            var resolveFunc = new Function(encResolveFunc);
            task.ContinueWith((task) =>
            {
				resolveFunc.Invoke();
				Task.Run(run_queued_promise_jobs);
            });
            return new Promise(encPromise);
        }
    }
}
