#pragma once
#include "Forward.h"
#include "Value.h"
#include <LibJS/Runtime/Value.h>
#include <LibJS/Runtime/Object.h>
#include <LibJS/Runtime/PropertyKey.h>
#include <LibJS/Runtime/FunctionObject.h>
#include <LibJS/Runtime/AbstractOperations.h>
#include <LibJS/Runtime/ExecutionContext.h>

extern "C" {

    EncodedValue js_object_get_property_value_at_index(EncodedValue object, int index);
    EncodedValue js_object_get_property_value(EncodedValue object, const char* name);

}