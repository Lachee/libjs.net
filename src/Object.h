#pragma once
#include "Forward.h"
#include "Value.h"
#include <LibJS/Runtime/Value.h>
#include <LibJS/Runtime/Object.h>
#include <LibJS/Runtime/PropertyKey.h>
#include <LibJS/Runtime/FunctionObject.h>
#include <LibJS/Runtime/AbstractOperations.h>
#include <LibJS/Runtime/ExecutionContext.h>

using PromiseCallback = void (*)(Value);


extern "C" {
    Value js_array_create(Value* values, size_t length);

    Value js_object_create_error(const char* message, const char* stack_trace);
    Value js_object_get_property_value_at_index(Value object, int index);
    Value js_object_get_property_value(Value object, const char* name);

    Value js_function_invoke(Value func, Value* arguments, size_t length);

    void js_promise_on_complete(Value encoded, PromiseCallback resolved, PromiseCallback rejected);
    Value js_promise_create(Value* resolve, Value* reject);

}