// See https://aka.ms/new-console-template for more information
// In your application's startup code
AppDomain.CurrentDomain.AssemblyLoad += (sender, args) => 
{
    Console.WriteLine($"Loaded Assembly: {args.LoadedAssembly.FullName}");
};

AppDomain.CurrentDomain.AssemblyResolve += (sender, args) => 
{
    Console.WriteLine($"Attempting to resolve: {args.Name}");
    return null; // Or provide a custom resolution mechanism
};


string dest = "./bin/Debug/net8.0";
foreach (string dependent in Directory.GetFiles("../build/release", "*.dll"))
{
    string dependentName = Path.GetFileName(dependent);
    System.IO.File.Copy(dependent, $"{dest}/{dependentName}", true);
}

string script = @"
    function add(a, b) {
        return a + b;
    }
    const result = add(1, 2);
    print('Result: ' + result);
";

Console.WriteLine("Running script: \n\n" + script );
var result = Bind.run_script(script);
Console.WriteLine($"Result: {result}");