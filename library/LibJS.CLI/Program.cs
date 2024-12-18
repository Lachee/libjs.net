﻿using LibJS;

ThreadPool.SetMaxThreads(1, 1);
List<Task> pendingTasks = new List<Task>();

var document = new LibJS.Document();
document.OnLog += (level, message) =>
{
    if (level == LibJS.LogLevel.Error)
    {
        Console.ForegroundColor = ConsoleColor.Red;
        Console.Error.WriteLine($":: [{level}] {message}");
    }
    else
    {
        if (level == LibJS.LogLevel.Log)
            Console.ForegroundColor = ConsoleColor.White;
        else if (level == LibJS.LogLevel.Warn)
            Console.ForegroundColor = ConsoleColor.Yellow;
        else if (level == LibJS.LogLevel.Info || level == LibJS.LogLevel.Debug)
            Console.ForegroundColor = ConsoleColor.Cyan;
        else if (level == LibJS.LogLevel.Trace)
            Console.ForegroundColor = ConsoleColor.Gray;
        else
            Console.ForegroundColor = ConsoleColor.Magenta;

        Console.WriteLine($":: [{level}] {message}");
    }
    Console.ResetColor();
};

document.DefineFunction("print", (env, args) =>
{
    Console.WriteLine("print:\t" + string.Join(" ", args.Select(a => a.ToString())));
});

var result = document.Evaluate(@"
    const a = 0.5;
    const b = 0.1;
    const c = a + b;
    console.log(a, '+', b, '=', c);
    print('hello world');
    () => console.log('apple');
");
Console.WriteLine("Result: {0}", result);

// This still throws the same issue.
Console.WriteLine("==========");
result.Invoke();

Console.WriteLine("==========");
Console.WriteLine("Done!");