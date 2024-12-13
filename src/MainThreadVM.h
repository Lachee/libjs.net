#pragma once

#include "Forward.h"

/** Initializes the global VM on the main thread */
ErrorOr<void> initialize_main_thread_vm();

/** Gets the global main thread VM */
JS::VM& main_thread_vm();

/** Creates a new realm */
NonnullOwnPtr<JS::ExecutionContext> create_execution_context(Function<JS::Object* (JS::Realm&)> create_global_object, Function<JS::Object* (JS::Realm&)> create_global_this_value);

