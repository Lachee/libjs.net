# LibJS.NET

> [!IMPORTANT]  
> This is a experiment and is not suppose to be a proper library. Use this at your own risk.

[![Build](https://github.com/Lachee/libjs.net/actions/workflows/build.yml/badge.svg)](https://github.com/Lachee/libjs.net/actions/workflows/build.yml)

LibJS.NET is a .NET Binding for the [Ladybird](https://github.com/LadybirdBrowser/ladybird) LibJS engine.

It uses the minimal required code to get LibJS building and running, so it is missing some features of the Ladybird engine.

## Why a seperate repo?

This is a **experimental** _library_. It is done as its own seperate repo as I feel that it is out of scope for Ladybird to support this weird edge case of embedding LibJS into .NET applications (or Unity games).

The changes I have made to Ladybird itself are done in patches. While they are small changes, I felt they are not nessarily high quality or appropriate to be commited back to the main Ladybird repository. 

## Deps
- clang-cl
- vcpkg
- python3
- cmake & ninja
- dotnet

## Building
> [!NOTE]  
> Building is done from the root project directory.

You can build this project on both Linux and Windows 👀
Use the scripts to initialize and build (bash and batch versions available).

- `initialize.sh` will pull submodules, prepare vcpkg, and apply any patches required.
- `build.sh` will build the Unmanaged and Managed libraries. They will all be outputted into `bin/`

## Development
If you are building a performant sensitive application, its recommended to use this project more as a base than a complete library into itself. Marshalling between Unmanaged (C++) and Managed (C#) code is not fast, especially when dealing with objects and strings.

Its recommend to implement your own functions and objects in C++ rather than C# to avoid this cost of marshalling. However, if you prefer to implement it in C#, this library provides functionality to define JS functions in C#.

## Documentation
Documentation is not yet implemented. Look at `library/LibJS.CLI/program.cs` for examples on implementation.

## Contribution
I am more than happy to accept contributions, but this project is still fairly small and unorganised. I am mainly looking at help with the Unmanaged side of things as im a beginner and still learning.

Suggestions and API requests are welcomed.

## License
This project will follow [Ladybird's License](https://github.com/LadybirdBrowser/ladybird/blob/master/LICENSE) and will mirror that. It is currently BSD 2-Clause.