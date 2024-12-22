#include "Object.h"
#include "Forward.h"
#include "MainThreadVM.h"
#include "ExternError.h"
#include "EnvironmentSettingsObject.h"
#include <LibJS/Runtime/Value.h>
#include <LibJS/Runtime/Object.h>
#include <LibJS/Runtime/PropertyKey.h>
#include <LibJS/Runtime/FunctionObject.h>
#include <LibJS/Runtime/AbstractOperations.h>
#include <LibJS/Runtime/ExecutionContext.h>
#include <LibJS/Runtime/Promise.h>
#include <LibJS/Runtime/NativeFunction.h>

extern "C" {

    EncodedValue js_object_create_error(const char* message, const char* stack_trace)
    {
        auto& vm = main_thread_vm();
        auto realm = vm.current_realm();
        auto error = ExternError::create(*realm, StringView{ message, strlen(message) }, StringView{ stack_trace, strlen(stack_trace) });
        return encode_js_value(error);
    }

    EncodedValue js_object_get_property_value_at_index(EncodedValue object, int index)
    {
        JS::PropertyKey key(index);
        auto result = decode_js_value(object).as_object().get(key);
        if (result.is_error()) {
            warnln("Error getting property with index");
            return JS::js_undefined().encoded();
        }
        return encode_js_value(result.value());
    }

    EncodedValue js_object_get_property_value(EncodedValue object, const char* name)
    {
        DeprecatedFlyString keyName(name);
        JS::PropertyKey key(keyName);
        auto result = decode_js_value(object).as_object().get(key);
        if (result.is_error()) {
            warnln("Error getting property with name");
            return JS::js_undefined().encoded();
        }
        return encode_js_value(result.value());
    }

    void js_object_then(EncodedValue encoded, void (*then)(EncodedValue))
    {
        auto result = decode_js_value(encoded);
        if (!is<JS::Promise>(result.as_object())) {
            warnln("Value is not a promise");
            return;
        }

        auto& promise = static_cast<JS::Promise&>(result.as_object());
        auto& realm = promise.shape().realm();
        auto& vm = realm.vm();

        // FIXME: Im not sure why we have to push the realm context onto the stack here.
        prepare_to_run_script(realm);

        auto on_fulfilled_steps = [then](JS::VM& vm) -> JS::ThrowCompletionOr<JS::Value> {
            auto value = vm.argument(0);
            then(encode_js_value(value));
            return JS::js_undefined();
            };

        auto on_fulfilled = JS::NativeFunction::create(realm, move(on_fulfilled_steps), 1, "");
        promise.perform_then(on_fulfilled, on_fulfilled, nullptr);

        clean_up_after_running_script(realm);
    }
}