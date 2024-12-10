#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
#   ifdef BUILDING_LIBJS
#       define LIBJS_API __declspec(dllexport)
#   else
#       define LIBJS_API __declspec(dllimport)
#   endif
#else
#   include <unistd.h>
#   define LIBJS_API extern "C"
#endif

#include <iostream>
#include <AK/HashMap.h>
#include <AK/Types.h>
#include <AK/StringView.h>

#ifdef USE_GLOBAL
extern "C" {
void register_global_function(const char *name, void *function);
int run_global_script(const char *data);
}
#endif
