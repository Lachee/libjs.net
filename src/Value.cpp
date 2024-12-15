#include "Value.h"
#include "MainThreadVM.h"
#include <LibJS/Runtime/Value.h>
#include <LibJS/Runtime/Object.h>
#include <LibJS/Runtime/PropertyKey.h>

extern "C" {
    void js_value_free(JS::Value* value)
    {
        delete value;
    }

    u16 js_value_tag(JS::Value* value)
    {
        return value->tag();
    }

    u64 js_value_encoded(JS::Value* value)
    {
        return value->encoded();
    }

    double js_value_as_double(JS::Value* value)
    {
        return value->as_double();
    }

    size_t js_value_to_string(JS::Value* value, char* result, size_t result_size)
    {
        auto& vm = main_thread_vm();
        auto str = value->to_string(vm);
        if (str.is_error()) {
            warnln("js_value_to_string: error");
            return 0;
        }

        auto sv = StringView{ str.value() };
        size_t length = sv.length();
        if (length >= result_size) {
            warnln("js_value_to_string: result buffer too small");
            return 0;
        }

        strncpy(result, sv.characters_without_null_termination(), length);
        result[length] = '\0'; // Null-terminate the result
        return length;
    }

    bool js_value_is_function(JS::Value* value)
    {
        return value->is_function();
    }

    bool js_value_is_constructor(JS::Value* value)
    {
        return value->is_constructor();
    }

    bool js_value_is_error(JS::Value* value)
    {
        return value->is_error();
    }

    JS::Value* js_object_get_property_value_at_index(JS::Object* object, int index) {
        JS::PropertyKey key(index);
        auto result = object->get(key);
        if (result.is_error())
            return nullptr;
        return new JS::Value(result.release_value());
    }
    JS::Value* js_object_get_property_value(JS::Object* object, const char* name) {
        DeprecatedFlyString keyName(name);
        JS::PropertyKey key(keyName);
        auto result = object->get(key);
        if (result.is_error())
            return nullptr;
        return new JS::Value(result.release_value());
    }

}