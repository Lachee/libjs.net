#if DEBUG
using LibJS.Types;

static void CopyLibraries()
{
	string sourcePath = Path.Combine("..", "..", "..", "..", "..", "Build", "release");
	string targetPath = AppDomain.CurrentDomain.BaseDirectory;

	foreach (var file in Directory.GetFiles(sourcePath, "*.*", SearchOption.TopDirectoryOnly)
		.Where(s => s.EndsWith(".dll") || s.EndsWith(".so")))
	{
		string fileName = Path.GetFileName(file);
		string destFile = Path.Combine(targetPath, fileName);
		Console.WriteLine($"-- copying {fileName}");
		File.Copy(file, destFile, true);
	}
}


// Call the function at the start of the Main method
CopyLibraries();
#endif

ThreadPool.SetMaxThreads(1, 1);
List<Task> pendingTasks = new List<Task>();

var document = new LibJS.Document();
var logger = LibJS.Intrinsics.Logger.Create(document);
var timers = LibJS.Intrinsics.Timers.Create(document);

document.DefineFunction("custom", (doc, args) =>
{
    throw new Exception("Test Exception");
});

Console.WriteLine("==========");
Console.WriteLine("Waiting constructed promise...");
var promise = Promise.Create(Task.Run(async () => {
	await Task.Delay(1000);
	return Value.Create(1000);
}));
promise.AsTask().Wait();

Console.WriteLine("==========");
var result = document.Evaluate(@"
    const a = 0.5;
    const b = 0.1;
    
    new Promise((resolve, reject) => {
        setTimeout(() => {
            console.log('Resolving Promise');
			resolve();
		}, 3000);
    });
");
Console.WriteLine("Result: {0}", result);

// This still throws the same issue.
Console.WriteLine("==========");
Console.WriteLine("Waiting Result...");
result.AsObject<Promise>().AsTask().Wait();

Console.WriteLine("Waiting Pending...");
timers.WaitPending().Wait();

Console.WriteLine("Done!");
