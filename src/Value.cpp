#include "Value.h"
#include "MainThreadVM.h"
#include <LibJS/Runtime/Value.h>
#include <LibJS/Runtime/Object.h>
#include <LibJS/Runtime/PropertyKey.h>
#include <LibJS/Runtime/FunctionObject.h>
#include <LibJS/Runtime/AbstractOperations.h>
#include <LibJS/Runtime/ExecutionContext.h>

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

    int js_value_to_string(JS::Value* value, char* buff, int buffSize)
    {
        auto& vm = main_thread_vm();
        auto str = value->to_string_without_side_effects();
        if (str.bytes().size() >= static_cast<size_t>(buffSize)) {
            warnln("-- Result buffer is too small: {} < {}", buffSize, str.bytes().size());
            return 0;
        }
        memcpy(buff, str.bytes().data(), str.bytes().size());
        return str.bytes().size();
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

    JS::Value* js_value_call(JS::Value* value)
    {
        // FIXME: Pass arguments to call
        if (!value) {
            warnln("Error: value is null");
            return nullptr;
        }

        if (!value->is_function()) {
            warnln("Error: value is not a function");
            return nullptr;
        }

        dbgln("-- Calling Function");
        JS::VM& vm = main_thread_vm();
        // Adding a new context doesnt fix this suee
        //  OwnPtr<JS::ExecutionContext> execution_context = JS::ExecutionContext::create();
        //  vm.push_execution_context(*execution_context);
        vm.dump_backtrace();

        dbgln("--- B: {}", value->to_string_without_side_effects());
        auto result = JS::call(vm, *value, JS::js_undefined(), vm.running_execution_context().arguments.span());

        dbgln("--- C");
        // vm.pop_execution_context();
        if (result.is_error()) {
            warnln("Error calling function");
            return nullptr;
        }
        return new JS::Value(result.release_value());
    }

    JS::Value* js_object_get_property_value_at_index(JS::Object* object, int index)
    {
        JS::PropertyKey key(index);
        auto result = object->get(key);
        if (result.is_error()) {
            warnln("Error getting property with index");
            return nullptr;
        }
        return new JS::Value(result.release_value());
    }

    JS::Value* js_object_get_property_value(JS::Object* object, const char* name)
    {
        DeprecatedFlyString keyName(name);
        JS::PropertyKey key(keyName);
        auto result = object->get(key);
        if (result.is_error()) {
            warnln("Error getting property with name");
            return nullptr;
        }
        return new JS::Value(result.release_value());
    }

}