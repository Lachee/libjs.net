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

    bool js_value_is_array(EncodedValue encoded)
    {
        auto result = decode_js_value(encoded).is_array(main_thread_vm());
        if (result.is_error()) {
            warnln("Error checking if value is array");
            return false;
        }
        return result.value();
    }

    bool js_value_is_regexp(EncodedValue encoded)
    {
        auto result = decode_js_value(encoded).is_regexp(main_thread_vm());
        if (result.is_error()) {
            warnln("Error checking if value is regexp");
            return false;
        }
        return result.value();
    }

    EncodedValue js_value_invoke(EncodedValue encoded)
    {
        auto value = decode_js_value(encoded);
        VERIFY(value.is_function());

        auto& function_object = value.as_function();
        auto& relevant_realm = function_object.shape().realm();

        prepare_to_run_script(relevant_realm);

        auto this_value = JS::js_undefined();
        auto& vm = function_object.vm();
        auto result = JS::call(vm, function_object, this_value);

        clean_up_after_running_script(relevant_realm);

        // vm.pop_execution_context();
        if (result.is_error()) {
            warnln("Error calling function");
            return encode_js_value(JS::js_undefined());
        }

        return encode_js_value(result.value());
    }
}