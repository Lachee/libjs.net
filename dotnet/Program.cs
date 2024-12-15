// Just for debugging, we copy the latest binaries.
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


var environment = new LibJS.Environment();
environment.OnLog += (level, message) =>
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

environment.DefineFunction("print", (env, args) =>
{
    Console.WriteLine("Custom function called 'print'");
    for(int i = 0; i < args.Count; i++) {
        Console.WriteLine("- Arg #{0}:\t{1}", i, args[i].ToString());
    }
});
var result = environment.Evaluate(@"
    const a = 0.5;
    const b = 0.1;
    const c = a + b;
    print('phill fish', 'is stinky');
    const promise = new Promise((resolve, reject) => {
        console.log('Promise Executions');
    });
    c;
");
Console.WriteLine($"ToString: {result}");
Console.WriteLine($"Double: {result.asDouble() * 2}");
Console.WriteLine("Press any key to exit...");