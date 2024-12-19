#include "Forward.h"
#include "Value.h"
#include "MainThreadVM.h"
#include "EnvironmentSettingsObject.h"
#include "Document.h"
#include <LibJS/Runtime/Value.h>
#include <LibJS/Runtime/Object.h>
#include <LibJS/Runtime/PropertyKey.h>
#include <LibJS/Runtime/FunctionObject.h>
#include <LibJS/Runtime/AbstractOperations.h>
#include <LibJS/Runtime/ExecutionContext.h>


JS::Value decode_js_value(EncodedValue encoded)
{
    return *reinterpret_cast<JS::Value*>(&encoded);
}

EncodedValue encode_js_value(JS::Value value)
{
    return value.encoded();
}

extern "C" {
    int js_value_to_string(EncodedValue encoded, char* buff, int buffSize)
    {
        auto value = decode_js_value(encoded);
        auto& vm = main_thread_vm();
        auto str = value.to_string_without_side_effects();
        dbgln("String: {}", str);
        if (str.bytes().size() >= static_cast<size_t>(buffSize)) {
            warnln("-- Result buffer is too small: {} < {}", buffSize, str.bytes().size());
            return 0;
        }
        memcpy(buff, str.bytes().data(), str.bytes().size());
        return str.bytes().size();
    }

    bool js_value_is_function(EncodedValue encoded)
    {
        return decode_js_value(encoded).is_function();
    }

    bool js_value_is_constructor(EncodedValue encoded)
    {
        return decode_js_value(encoded).is_constructor();
    }

    bool js_value_is_error(EncodedValue encoded)
    {
        return decode_js_value(encoded).is_error();
    }

    EncodedValue js_object_get_property_value_at_index(JS::Object* object, int index)
    {
        JS::PropertyKey key(index);
        auto result = object->get(key);
        if (result.is_error()) {
            warnln("Error getting property with index");
            return JS::js_undefined().encoded();
        }
        return encode_js_value(result.value());
    }

    EncodedValue js_object_get_property_value(JS::Object* object, const char* name)
    {
        DeprecatedFlyString keyName(name);
        JS::PropertyKey key(keyName);
        auto result = object->get(key);
        if (result.is_error()) {
            warnln("Error getting property with name");
            return JS::js_undefined().encoded();
        }
        return encode_js_value(result.value());
    }
}