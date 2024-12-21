using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection.Metadata;
using System.Text;
using System.Threading.Tasks;

namespace LibJS.Intrinsics
{
	public class Logger : IIntrinsic
	{
		public bool UseColor { get; set; } = true;
		public string Prefix { get; set; } = "";

		public void Register(Document document)
		{
			document.OnLog += OnLog;
		}

		public static Logger Create(Document document)
		{
			var logger = new Logger();
			logger.Register(document);
			return logger;
		}

		private void OnLog(LogLevel level, string message)
		{
			string formatted = Format(level, message);
			if (!UseColor) 
			{
				Console.WriteLine(formatted);
				return;
			}

			Console.ForegroundColor = level switch
			{
				LogLevel.Error => ConsoleColor.Red,
				LogLevel.Log => ConsoleColor.White,
				LogLevel.Warn => ConsoleColor.Yellow,
				LogLevel.Info or LibJS.LogLevel.Debug => ConsoleColor.Cyan,
				LogLevel.Trace => ConsoleColor.Gray,
				_ => ConsoleColor.Magenta
			};
				
			Console.WriteLine(formatted);			
			Console.ResetColor();
		}

		public virtual string Format(LogLevel logLevel, string message) 
		{
			if (UseColor) return message;
			return $"{Prefix} [{logLevel}] {message}";
		}
	}
}
