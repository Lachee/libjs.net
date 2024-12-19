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

JS::Value decode_js_value(EncodedValue encoded);
EncodedValue encode_js_value(JS::Value value);

extern "C" {
    // int js_value_to_string(JS::Value value, char* buff, int buffSize);
    int js_value_to_string(EncodedValue value, char* buff, int buffSize);

    bool js_value_is_function(EncodedValue encoded);
    bool js_value_is_constructor(EncodedValue encoded);
    bool js_value_is_error(EncodedValue encoded);

    EncodedValue js_object_get_property_value_at_index(JS::Object* object, int index);
    EncodedValue js_object_get_property_value(JS::Object* object, const char* name);

    EncodedValue js_value_invoke(EncodedValue encoded);
}