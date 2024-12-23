﻿using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using LibJS.Types;

namespace LibJS.Intrinsics
{
    public class Timers : IIntrinsic
	{
		private int m_lastTimerId = 1;
		private Dictionary<int, Task> m_timers;

		public Timers(int capacity = 100) 
		{
			m_timers = new Dictionary<int, Task>(capacity);
		}

		/// <summary>
		/// Creates a new Timer intrinsic and registers it to the document.
		/// </summary>
		/// <param name="document"></param>
		/// <returns></returns>
		public static Timers Create(Document document)
		{
			Timers timers = new Timers();
			timers.Register(document);
			return timers;
		}

		public void Register(Document document)
		{
			document.DefineFunction("setTimeout", SetTimeout);
			document.DefineFunction("setInterval", SetInterval);
			document.DefineFunction("clearTimeout", ClearTimeout);
			document.DefineFunction("clearInterval", ClearTimeout);
		}


		/// <summary>
		/// Waits for all pending timers to finish
		/// </summary>
		public async Task WaitPending(CancellationToken cancellationToken = default)
		{
			while(m_timers.Count > 0 || cancellationToken.IsCancellationRequested)
			{
				await m_timers.First().Value.WaitAsync(cancellationToken);
			}
		}

		/// <summary>
		/// Clears all timers
		/// </summary>
		public void ClearAll()
		{
			m_timers.Clear();
		}

		private Value SetTimeout(Document doc, Types.Array args) 
		{
			if (args.Count != 2)
				throw new ArgumentException("SetTimeout expects 2 arguments");

			var callback = args[0];
			if (!callback.IsFunction)
				throw new ArgumentException("First argument must be a callback.");
			
			var milliseconds = args[1];
			if (!milliseconds.IsNumber)
				throw new ArgumentException("Second argument must be milliseconds.");

			int id = m_lastTimerId++;
			m_timers.Add(id, Task.Run(async () =>
			{
				await Task.Delay((int)Math.Max(Math.Floor(milliseconds.AsDouble()), 1));
				if (m_timers.Remove(id)) {
					callback.AsObject<Function>().Invoke();
				}
			}));

			return Value.Create(id);
		}

		private Value SetInterval(Document doc, Types.Array args)
		{
			if (args.Count != 2)
				throw new ArgumentException("SetTimeout expects 2 arguments");

			var callback = args[0];
			if (!callback.IsFunction)
				throw new ArgumentException("First argument must be a callback.");

			var milliseconds = args[1];
			if (!milliseconds.IsNumber)
				throw new ArgumentException("Second argument must be milliseconds.");

			int id = m_lastTimerId++;
			m_timers.Add(id, Task.Run(async () =>
			{
				while (true)
				{
					await Task.Delay((int)Math.Max(Math.Floor(milliseconds.AsDouble()), 1));
					if (m_timers.ContainsKey(id))
					{
						callback.AsObject<Function>().Invoke();
					}
					else
					{
						break;
					}
				}
			}));

			return Value.Create(id);
		}

		private void ClearTimeout(Document doc, Types.Array args)
		{
			if (args.Count != 1)
				throw new ArgumentException("SetTimeout expects 1 argument");

			var callbackId = args[0];
			if (!callbackId.IsInt32)
				throw new ArgumentException("First argument must be a callback id.");

			m_timers.Remove(callbackId.AsInt());
		}
	
	}
}
