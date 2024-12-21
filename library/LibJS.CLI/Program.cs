
ThreadPool.SetMaxThreads(1, 1);
List<Task> pendingTasks = new List<Task>();

var document = new LibJS.Document();
var timers = LibJS.Intrinsics.Timers.Create(document);

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

document.DefineFunction("print", (doc, args) =>
{
    Console.WriteLine("print:\t" + string.Join(" ", args.Select(a => a.ToString())));
});


var result = document.Evaluate(@"
    const a = 0.5;
    const b = 0.1;
    const c = a + b;
    const intervalId = setInterval(() => {
        console.log('Hello from an interval!');
	}, 1_000);

    console.log('interval id:', intervalId);

    setTimeout(() => {
        console.log('Hello from a delayed call! Cancelling interval.');
        clearInterval(intervalId);
    }, 3_000);
");
Console.WriteLine("Result: {0}", result);

// This still throws the same issue.
Console.WriteLine("==========");

Console.WriteLine("Waiting Pending...");
timers.WaitPending().Wait();

Console.WriteLine("Done!");