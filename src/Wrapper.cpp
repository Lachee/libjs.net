#include "Wrapper.h"

#include <AK/StringView.h>
#include <iostream>

#ifdef USE_GLOBAL
#include "LibJS.h"
extern "C" {

void register_global_function(const char *name, void *function) {
  StringView view = StringView{name, strlen(name)};
  auto function_name = view.hash();
  ::function_map.set(function_name, reinterpret_cast<void (*)(void)>(function));
}

int run_global_script(const char *data) {
  auto result_or_error = try_run_script(data);
  if (result_or_error.is_error())
    return -1;
  return result_or_error.value();
}
}
#endif