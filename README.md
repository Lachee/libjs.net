# LibJS.NET

> [!IMPORTANT]  
> This is a experiment and is not suppose to be a proper library. Use this at your own risk.

LibJS.NET is a .NET Binding for the [Ladybird](https://github.com/LadybirdBrowser/ladybird) LibJS engine.

It uses the minimal required code to get LibJS building and running, so it is missing some features of the Ladybird engine and some JS standards might straight up be disabled (like module imports).

This is a **experimental** _library_. It is done as its own seperate repo as I feel that it is out of scope for Ladybird to support this weird edge case of embedding LibJS into .NET applications (or Unity games).

The changes I have made to Ladybird itself are done in patches. While they are small changes, I felt they are not nessarily high quality or appropriate to be commited back to the main Ladybird repository. 

Look at the `Libraries/LibJS/Runtime/VM.cpp` patch as an example, it straight up deletes the import_modules because i didnt want to import File.

## Deps
- clang-cl
- vcpkg
- python3
- cmake & ninja
- dotnet

## Building
> [!NOTE]  
> Building is done from the root project directory.

1. initialize the project `initialize.bat`
2. build the project `build.bat`
3. profit?

