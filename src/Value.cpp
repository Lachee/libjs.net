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

#if 0
JS::ExecutionContext const& execution_context_of_realm(JS::Realm const& realm) {
    VERIFY(realm.host_defined());

    // 1. If realm is a principal realm, then return the realm execution context of the environment settings object of realm.
    if (is<Bindings::PrincipalHostDefined>(*realm.host_defined()))
        return static_cast<Bindings::PrincipalHostDefined const&>(*realm.host_defined()).environment_settings_object->realm_execution_context();

    // 2. Assert: realm is a synthetic realm.
    // 3. Return the execution context of the synthetic realm settings object of realm.
    return *verify_cast<Bindings::SyntheticHostDefined>(*realm.host_defined()).synthetic_realm_settings.execution_context;
}

inline JS::ExecutionContext& execution_context_of_realm(JS::Realm& realm) {
    return const_cast<JS::ExecutionContext&>(execution_context_of_realm(const_cast<JS::Realm const&>(realm)));
}
#endif

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

    JS::Value* js_value_call(Document* environment, JS::Value* value)
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

        auto& function_object = value->as_function();
        auto& relevant_realm = function_object.shape().realm();

        prepare_to_run_script(relevant_realm);

        dbgln("-> Calling");
        auto this_value = JS::js_undefined();
        auto& vm = function_object.vm();
        auto result = JS::call(vm, function_object, this_value);

        clean_up_after_running_script(relevant_realm);

        // vm.pop_execution_context();
        if (result.is_error()) {
            warnln("Error calling function");
            return new JS::Value(JS::js_undefined());
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