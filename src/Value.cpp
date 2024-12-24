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


JS::Value decode(Value encoded)
{
    return *reinterpret_cast<JS::Value*>(&encoded);
}

Value encode(JS::Value value)
{
    return value.encoded();
}

extern "C" {
    int js_value_to_string(Value encoded, char* buff, int buffSize)
    {
        auto value = decode(encoded);
        auto& vm = main_thread_vm();
        auto str = value.to_string_without_side_effects();
        if (str.bytes().size() >= static_cast<size_t>(buffSize)) {
            warnln("-- Result buffer is too small: {} < {}", buffSize, str.bytes().size());
            return 0;
        }
        memcpy(buff, str.bytes().data(), str.bytes().size());
        return str.bytes().size();
    }

    bool js_value_is_function(Value encoded)
    {
        return decode(encoded).is_function();
    }

    bool js_value_is_constructor(Value encoded)
    {
        return decode(encoded).is_constructor();
    }

    bool js_value_is_error(Value encoded)
    {
        return decode(encoded).is_error();
    }

    bool js_value_is_array(Value encoded)
    {
        auto result = decode(encoded).is_array(main_thread_vm());
        if (result.is_error()) {
            warnln("Error checking if value is array");
            return false;
        }
        bool is_array = result.value();
        return is_array;
    }

    bool js_value_is_regexp(Value encoded)
    {
        auto result = decode(encoded).is_regexp(main_thread_vm());
        if (result.is_error()) {
            warnln("Error checking if value is regexp");
            return false;
        }
        return result.value();
    }

    bool js_value_is_promise(Value encoded)
    {
        auto value = decode(encoded);
        if (value.is_object() && is<JS::Promise>(value.as_object()))
            return true;
        return false;
    }
}