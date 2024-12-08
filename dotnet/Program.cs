// See https://aka.ms/new-console-template for more information

string script = @"
    function add(a, b) {
        return a + b;
    }
    const result = add(1, 2);
    print(result);
";

Console.WriteLine("Running script...");
var result = Bind.run_script(script);
Console.WriteLine($"Result: {result}");