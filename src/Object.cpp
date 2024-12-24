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
#include <LibJS/Runtime/Array.h>


extern "C" {

    Value js_array_create(Value* values, size_t length)
    {
        auto& vm = main_thread_vm();
        auto realm = vm.current_realm();
        auto array = MUST(JS::Array::create(*realm, length));

        for (size_t i = 0; i < length; i++)
            array->indexed_properties().put(i, decode(values[i]));

        return encode(JS::Value(array));
    }

    Value js_object_create_error(const char* message, const char* stack_trace)
    {
        auto& vm = main_thread_vm();
        auto realm = vm.current_realm();
        auto error = ExternError::create(*realm, StringView{ message, strlen(message) }, StringView{ stack_trace, strlen(stack_trace) });
        return encode(error);
    }

    Value js_object_get_property_value_at_index(Value object, int index)
    {
        JS::PropertyKey key(index);
        auto result = decode(object).as_object().get(key);
        if (result.is_error()) {
            warnln("Error getting property with index");
            return JS::js_undefined().encoded();
        }
        return encode(result.value());
    }

    Value js_object_get_property_value(Value object, const char* name)
    {
        DeprecatedFlyString keyName(name);
        JS::PropertyKey key(keyName);
        auto result = decode(object).as_object().get(key);
        if (result.is_error()) {
            warnln("Error getting property with name");
            return JS::js_undefined().encoded();
        }
        return encode(result.value());
    }

    Value js_function_invoke(Value func, Value* arguments, size_t length)
    {
        auto value = decode(func);
        VERIFY(value.is_function());

        AK::Span<JS::Value> args{ reinterpret_cast<JS::Value*>(arguments), length };

        auto& function_object = value.as_function();
        auto& relevant_realm = function_object.shape().realm();

        prepare_to_run_script(relevant_realm);

        auto this_value = JS::js_undefined();
        auto& vm = function_object.vm();
        auto result = JS::call(vm, function_object, this_value, args);

        clean_up_after_running_script(relevant_realm);

        // vm.pop_execution_context();
        if (result.is_error()) {
            warnln("Error calling function");
            return encode(JS::js_undefined());
        }

        return encode(result.value());
    }

    void js_promise_on_complete(Value encoded, PromiseCallback resolved, PromiseCallback rejected)
    {
        auto result = decode(encoded);
        if (!is<JS::Promise>(result.as_object())) {
            warnln("Value is not a promise");
            return;
        }

        auto& promise = static_cast<JS::Promise&>(result.as_object());
        auto& realm = promise.shape().realm();
        auto& vm = realm.vm();

        // FIXME: Im not sure why we have to push the realm context onto the stack here.
        prepare_to_run_script(realm);

        auto on_fulfilled_steps = [resolved](JS::VM& vm) -> JS::ThrowCompletionOr<JS::Value> {
            auto value = vm.argument(0);
            resolved(encode(value));
            return JS::js_undefined();
            };

        auto on_failure_steps = [rejected](JS::VM& vm) -> JS::ThrowCompletionOr<JS::Value> {
            auto value = vm.argument(0);
            rejected(encode(value));
            return JS::js_undefined();
            };

        auto on_fulfilled = JS::NativeFunction::create(realm, move(on_fulfilled_steps), 1, "");
        auto on_rejected = JS::NativeFunction::create(realm, move(on_failure_steps), 1, "");
        promise.perform_then(on_fulfilled, on_rejected, nullptr);

        clean_up_after_running_script(realm);
    }

    Value js_promise_create(Value* resolve, Value* reject)
    {
        auto& vm = main_thread_vm();
        auto realm = vm.current_realm();
        auto promise = JS::Promise::create(*realm);
        auto resolvingFunctions = promise->create_resolving_functions();
        auto resolveValue = JS::Value(resolvingFunctions.resolve);
        auto rejectValue = JS::Value(resolvingFunctions.reject);
        *resolve = encode(resolveValue);
        *reject = encode(rejectValue);
        return encode(promise);
    }
}