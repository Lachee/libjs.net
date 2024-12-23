#pragma once

#include "Forward.h"
#include <AK/Types.h>
#include <LibGC/Ptr.h>

JS::Value decode_js_value(EncodedValue encoded);
EncodedValue encode_js_value(JS::Value value);

extern "C" {
    // int js_value_to_string(JS::Value value, char* buff, int buffSize);
    int js_value_to_string(EncodedValue value, char* buff, int buffSize);

    bool js_value_is_function(EncodedValue encoded);
    bool js_value_is_constructor(EncodedValue encoded);
    bool js_value_is_error(EncodedValue encoded);
    bool js_value_is_array(EncodedValue encoded);
    bool js_value_is_regexp(EncodedValue encoded);
    bool js_value_is_promise(EncodedValue encoded);
}