#pragma once

#include "Forward.h"
#include <AK/Types.h>
#include <LibGC/Ptr.h>

class InteropValue {
private:
    GC::Ref<JS::Value> m_value;

public:
    // static InteropValue* create(JS::Value value);
    // JS::Value value();
};


extern "C" {
    void js_value_free(JS::Value* value);
    u16 js_value_tag(JS::Value* value);
    u64 js_value_encoded(JS::Value* value);
    size_t js_value_to_string(JS::Value* value, char* result, size_t result_size);
    double js_value_as_double(JS::Value* value);
    bool js_value_is_function(JS::Value* value);
    bool js_value_is_constructor(JS::Value* value);
    bool js_value_is_error(JS::Value* value);
}