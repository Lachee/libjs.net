
ThreadPool.SetMaxThreads(1, 1);
List<Task> pendingTasks = new List<Task>();

var document = new LibJS.Document();
var logger = LibJS.Intrinsics.Logger.Create(document);
var timers = LibJS.Intrinsics.Timers.Create(document);

document.DefineFunction("custom", (doc, args) =>
{
    throw new Exception("Test Exception");
    Console.WriteLine("custom function:\t" + string.Join(" ", args.Select(a => a.ToString())));
});

var result = document.Evaluate(@"
    const a = 0.5;
    const b = 0.1;
    const c = a + b;
    custom(a, b, c);

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