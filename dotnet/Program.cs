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

// Register the callback "test"
if (false)
{
    LibJS.Global.RegisterFunction("test", () =>
    {
        Console.WriteLine("We got a callback from C++!");
    });

    string script = @"
    const result = 420 - 351;
    print('Result: ' + result);
    invoke('test');
";
    Console.WriteLine("\n--Executing script...");
    var result = LibJS.Global.RunScript(script);
    Console.WriteLine("--Script Executed with status: {0}", result);
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

environment.Run(@"
    const result = 420 - 351;
    let second = result + 1;
    let third = result - 1;
    console.log({result});
    console.error('STICKY NESS!');
");