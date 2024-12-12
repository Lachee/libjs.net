#pragma once

#include "Forward.h"

#include "StandardIOStream.h"
#include <LibJS/Print.h>


/** Initializes the global VM on the main thread */
ErrorOr<void> initialize_main_thread_vm();

/** Gets the global main thread VM */
JS::VM& main_thread_vm();

/** Creates a new realm */
NonnullOwnPtr<JS::ExecutionContext> create_execution_context(Function<JS::Object* (JS::Realm&)> create_global_object, Function<JS::Object* (JS::Realm&)> create_global_this_value);

StandardIOStream stdout_stream(StreamType::Output);
StandardIOStream stderr_stream(StreamType::Error);

enum class PrintTarget {
    StandardError,
    StandardOutput,
};

static ErrorOr<void> print(JS::Value value, Stream& stream) {
    JS::PrintContext print_context{
        .vm = main_thread_vm(), .stream = stream, .strip_ansi = true };
    return JS::print(value, print_context);
}

static ErrorOr<void> print(JS::Value value,
    PrintTarget target = PrintTarget::StandardOutput) {
    auto& stream =
        target == PrintTarget::StandardError ? stderr_stream : stdout_stream;
    return print(value, stream);
}
