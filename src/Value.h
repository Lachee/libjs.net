#pragma once

#include "Forward.h"
#include <AK/Types.h>
#include <LibGC/Ptr.h>

JS::Value decode(Value encoded);
Value encode(JS::Value value);

extern "C" {
    // int js_value_to_string(JS::Value value, char* buff, int buffSize);
    int js_value_to_string(Value value, char* buff, int buffSize);

    bool js_value_is_function(Value encoded);
    bool js_value_is_constructor(Value encoded);
    bool js_value_is_error(Value encoded);
    bool js_value_is_array(Value encoded);
    bool js_value_is_regexp(Value encoded);
    bool js_value_is_promise(Value encoded);
}