// Just for debugging, we copy the latest binaries.
using LibJS;

string dest = "./bin/Debug/net8.0";
foreach (string dependent in Directory.GetFiles("../Build/release"))
{
    string extension = Path.GetExtension(dependent);
    if (extension == ".dll" || extension == ".so")
    {
        string dependentName = Path.GetFileName(dependent);
        System.IO.File.Copy(dependent, $"{dest}/{dependentName}", true);
    }
}

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

Value setTimeoutCallback = null;
document.DefineFunction("setTimeout", (env, args) =>
{
    if (args.Count != 2)
        throw new ArgumentException("setTimeout requires 2 arguments");
    if (!args[0].IsFunction)
        throw new ArgumentException("First argument must be a function");
    if (!args[1].IsNumber)
        throw new ArgumentException("Second argument must be a number");

    var callback = args[0];//.AsFunction;
    var milliseconds = args[1].AsDouble();
    Console.WriteLine($"setTimeout: CB: ${callback}, MS: ${milliseconds}");

    document.Call(callback);
    setTimeoutCallback = callback; // <- Invoking this later throws a AccessViolationException.
    // pendingTasks.Add(Task.Delay(TimeSpan.FromMilliseconds(milliseconds)).ContinueWith(_ => callback.Invoke())); <- This throws AccessViolationException
});

var result = document.Evaluate(@"
    const a = 0.5;
    const b = 0.1;
    const c = a + b;
    setTimeout(() => console.log('timeout called'), 1000);
    () => console.log('apple');
");

Console.WriteLine("Result: {0}", result);

// This still throws the same issue.
Console.WriteLine("==========");
if (setTimeoutCallback != null)
    document.Call(setTimeoutCallback);

document.Evaluate(@"
    console.log('Hello World!', a, b, c);
    apple();
");

Console.WriteLine("Waiting for tasks to finish...");
await Task.WhenAll(pendingTasks);

Console.WriteLine("Done!");