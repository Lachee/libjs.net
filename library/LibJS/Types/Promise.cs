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
        [DllImport(Consts.LibraryName)] private static extern void js_object_promise_invoke_on_complete(ulong value, IntPtr then);
        [DllImport(Consts.LibraryName)] private static extern Value js_object_promise_create(out Value resolve, out Value reject);
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
            => Create(task.ContinueWith(t => Value.Undefined));

        public static Promise Create(Task<Value> task)
        {
            var promise = js_object_promise_create(out var resolveValue, out var rejectValue);
            _ = Task.Run(async () =>
            {
                try
                {
                    var value = await task.ConfigureAwait(false);
                    resolveValue.AsObject<Function>().Invoke(value);
                }
                catch (Exception e)
                {
                    var error = Object.CreateError(e);
                    rejectValue.AsObject<Function>().Invoke(error.Value);
                }
                finally 
                {
					run_queued_promise_jobs();
				}
			});
            return new Promise(promise);
        }
    }
}
