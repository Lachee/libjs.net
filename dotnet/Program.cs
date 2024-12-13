﻿// Just for debugging, we copy the latest binaries.
string dest = "./bin/Debug/net8.0";
foreach (string dependent in Directory.GetFiles("../Build/release"))
{
    string extension = Path.GetExtension(dependent);
    if (extension == ".dll" || extension == ".so") {
        string dependentName = Path.GetFileName(dependent);
        System.IO.File.Copy(dependent, $"{dest}/{dependentName}", true);
    }
}

// Register the callback "test"
if (false) {
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
environment.OnInvoke += () =>
{
    Console.WriteLine("We got a callback from C++!");
};

environment.Run(@"
    const result = 420 - 351;
    let second = result + 1;
    let third = result - 1;
    console.log({result});
");