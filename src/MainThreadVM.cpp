#include "Forward.h"
#include "MainThreadVM.h"

static RefPtr<JS::VM> s_main_thread_vm;

ErrorOr<void> initialize_main_thread_vm()
{
    dbgln("-- Creating a main thread vm");
    VERIFY(!s_main_thread_vm);

    s_main_thread_vm = TRY(JS::VM::create());
    s_main_thread_vm->on_unimplemented_property_access = [](auto const& object, auto const& property_key) {
        warnln("-- FIXME: Unimplemented IDL interface: '{}.{}'", object.class_name(), property_key.to_string());
        };


    // NOTE: We intentionally leak the main thread JavaScript VM.
    //       This avoids doing an exhaustive garbage collection on process exit.
    // - I dont know what this means - Lake.
    s_main_thread_vm->ref();

    // auto& custom_data =
    // verify_cast<WebEngineCustomData>(*s_main_thread_vm->custom_data());
    // custom_data.event_loop =
    // s_main_thread_vm->heap().allocate<HTML::EventLoop>(type);

    return {};
}

JS::VM& main_thread_vm() {
    if (s_main_thread_vm.is_null()) {
        dbgln("-- No main thread vm, creating one");
        MUST(initialize_main_thread_vm());
    }

    VERIFY(s_main_thread_vm);
    return *s_main_thread_vm;
}

NonnullOwnPtr<JS::ExecutionContext> create_a_new_javascript_realm(Function<JS::Object* (JS::Realm&)> create_global_object, Function<JS::Object* (JS::Realm&)> create_global_this_value) {
    auto& vm = main_thread_vm();

    // 1. Perform InitializeHostDefinedRealm() with the provided customizations for creating the global object and the global this binding.
    // 2. Let realm execution context be the running JavaScript execution context.
    auto realm_execution_context = MUST(JS::Realm::initialize_host_defined_realm(vm, move(create_global_object), move(create_global_this_value)));

    // 3. Remove realm execution context from the JavaScript execution context stack.
    vm.execution_context_stack().remove_first_matching([&realm_execution_context](auto execution_context) {
        return execution_context == realm_execution_context.ptr();
        });

    // NO-OP: 4. Let realm be realm execution context's Realm component.
    // NO-OP: 5. Set realm's agent to agent.

    // FIXME: 6. If agent's agent cluster's cross-origin isolation mode is "none", then:
    //          1. Let global be realm's global object.
    //          2. Let status be ! global.[[Delete]]("SharedArrayBuffer").
    //          3. Assert: status is true.

    // 7. Return realm execution context.
    return realm_execution_context;
}