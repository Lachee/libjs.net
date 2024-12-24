#pragma once
#include "Forward.h"
#include "Value.h"
#include <LibJS/Runtime/Value.h>
#include <LibJS/Runtime/Object.h>
#include <LibJS/Runtime/PropertyKey.h>
#include <LibJS/Runtime/FunctionObject.h>
#include <LibJS/Runtime/AbstractOperations.h>
#include <LibJS/Runtime/ExecutionContext.h>

using PromiseCallback = void (*)(EncodedValue);

extern "C" {
    EncodedValue js_array_create(EncodedValue* values, size_t length);

    EncodedValue js_object_create_error(const char* message, const char* stack_trace);
    EncodedValue js_object_get_property_value_at_index(EncodedValue object, int index);
    EncodedValue js_object_get_property_value(EncodedValue object, const char* name);

    EncodedValue js_function_invoke(EncodedValue func, EncodedValue* arguments, size_t length);

    void js_promise_on_complete(EncodedValue encoded, PromiseCallback resolved, PromiseCallback rejected);
    EncodedValue js_promise_create(EncodedValue* resolve, EncodedValue* reject);
}