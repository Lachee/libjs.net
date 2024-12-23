using System;

namespace LibJS.Intrinsics
{
    public class Logger : IIntrinsic
	{
		public bool StripAnsi { get; set; } = false;
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
				LogLevel.Trace => ConsoleColor.Cyan,
				_ => ConsoleColor.Magenta
			};

			Console.WriteLine(formatted);
			Console.ResetColor();
		}

		public virtual string Format(LogLevel logLevel, string message) 
		{
			if (StripAnsi)
				message = StripAnsiColors(message);

			if (UseColor) return message;
			return $"{Prefix}[{logLevel}] {message}";
		}

		public static string StripAnsiColors(string input)
		{
			var ansiRegex = new System.Text.RegularExpressions.Regex(@"\x1B\[[0-?]*[ -/]*[@-~]");
			return ansiRegex.Replace(input, string.Empty);
		}
	}
}
