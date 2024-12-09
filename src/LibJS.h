#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
  #ifdef BUILDING_LIBJS
    #define LIBJS_API __declspec(dllexport)
  #else
    #define LIBJS_API __declspec(dllimport)
  #endif
#else
  #define LIBJS_API
#endif

#if !defined(WIN32)
#    include <unistd.h>
#else
#    include <WinSock2.h>
#    define STDIN_FILENO _fileno(stdin)
#    define STDOUT_FILENO _fileno(stdout)
#    define STDERR_FILENO _fileno(stderr)
#endif

#include <AK/Error.h>

ErrorOr<int> try_run_script(const char *data);

extern "C" void register_function(const char* name, void* function);
extern "C" int run_script(const char* data);