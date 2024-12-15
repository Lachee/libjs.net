#include "Forward.h"
#include "Environment.h"
#include "Window.h"
#include "MainThreadVM.h"
#include "LogClient.h"

#include <AK/Format.h>
#include <AK/Try.h>
#include <LibJS/Bytecode/Interpreter.h>
#include <LibJS/Runtime/VM.h>
#include <LibJS/Runtime/Array.h>
#include <LibJS/Runtime/ConsoleObject.h>
#include <LibJS/Runtime/PropertyKey.h>

Environment::Environment(JS::Realm& realm)
    : JS::GlobalObject(realm),
    m_realm(realm)
{
}

Environment::~Environment() = default;

GC::Ref<Environment> Environment::create(JS::Realm& realm)
{
    return realm.create<Environment>(realm);
}

void Environment::initialize(JS::Realm& realm)
{
    Base::initialize(realm);
}

GC::Ref<Environment> Environment::create_and_initialize()
{
    dbgln("-- Creating and initializing environment");

    GC::Ptr<GameWindow> window;

    auto execution_context = create_execution_context(
        [&](JS::Realm& realm) -> JS::Object* {
            window = GameWindow::create(realm);
            return window;
        }, nullptr);


    // 6. Set window to the global object of realmExecutionContext's Realm component.
    // Im sure this is here for a reason. Ladybird does it so I will too - Lake.
    window = verify_cast<GameWindow>(execution_context->realm->global_object());

    auto environment = Environment::create(window->realm());
    environment->m_window = window;
    window->set_associated_environment(environment);

    // Root level execution context
    auto module_execution_context = JS::ExecutionContext::create();
    module_execution_context->realm = window->realm();
    main_thread_vm().push_execution_context(*module_execution_context);
    return environment;
}

bool Environment::log(JS::Console::LogLevel log_level, StringView content)
{
    if (m_on_console_log_ptr) {
        m_on_console_log_ptr(log_level, content.characters_without_null_termination());
        return true;
    }
    return false;
}

ErrorOr<JS::Value> Environment::evaluate(StringView source, StringView source_name)
{
    dbgln("-- Parsing and running '{}'", source_name);

    auto& realm = window()->realm();
    auto& vm = realm.vm();


    auto& console_object = *realm.intrinsics().console_object();
    LogClient console_client(console_object.console(), *this);
    console_object.console().set_client(console_client);


    JS::ThrowCompletionOr<JS::Value> result{ JS::js_undefined() };

    auto run_script_or_module = [&](auto& script_or_module) {
        result = vm.bytecode_interpreter().run(*script_or_module);
        };

    // Execution
    auto script_or_error = JS::Script::parse(source, realm, source_name);
    if (script_or_error.is_error()) {
        auto error = script_or_error.error()[0];
        auto hint = error.source_location_hint(source);
        if (!hint.is_empty())
            outln("{}", hint);

        auto error_string = error.to_string();
        outln("{}", error_string);
        result = vm.throw_completion<JS::SyntaxError>(move(error_string));
    }
    else {
        auto script = script_or_error.value();
        run_script_or_module(script);
    }


    // Error Handling
    auto handle_exception = [&](JS::Value thrown_value) -> ErrorOr<void> {
        warnln("Uncaught exception:");
        // TODO: Fix This
        auto strval = MUST(thrown_value.to_string(vm));
        warnln("{}", strval);
        warnln();

        if (!thrown_value.is_object() || !is<JS::Error>(thrown_value.as_object()))
            return {};
        warnln("{}", static_cast<JS::Error const&>(thrown_value.as_object())
            .stack_string(JS::CompactTraceback::Yes));
        return {};
        };


    if (result.is_error()) {
        VERIFY(result.throw_completion().value().has_value());
        TRY(handle_exception(*result.release_error().value()));
        return Error::from_string_literal("Uncaught exception");
    }

    return result.value();
}

extern "C" {
    Environment* environment_create() {
        return Environment::create_and_initialize().ptr();
    }

    JS::Value* environmnet_evaluate(Environment* enviornment, const char* source, const char* source_name) {
        auto run_or_errored = enviornment->evaluate(StringView{ source, strlen(source) }, StringView{ source_name, strlen(source_name) });
        if (run_or_errored.is_error()) {
            warnln("Failed to evaluate script: {}", run_or_errored.error().string_literal());
            return nullptr;
        }

        return new JS::Value(run_or_errored.value());
    }

    void environment_set_on_console_log(Environment* environment, void (*on_console_log)(JS::Console::LogLevel, const char*)) {
        environment->set_on_console_log(Function<void(JS::Console::LogLevel, const char*)>(on_console_log));
    }

    void environment_define_function(Environment* environment, const char* name, void (*function)(JS::Array&)) {
        auto window = environment->window();
        auto& realm = window->realm();

        DeprecatedFlyString keyName(name);
        JS::PropertyKey key(keyName);

        auto function_object = AK::Function<JS::ThrowCompletionOr<JS::Value>(JS::VM&)>([function](JS::VM& vm) -> JS::ThrowCompletionOr<JS::Value> {
            dbgln("-- Running function");
            auto count = vm.argument_count();
            auto& realm = *vm.current_realm();
            auto arguments = TRY(JS::Array::create(realm, 0));
            for (size_t i = 0; i < count; i++) {
                auto argument = vm.argument(i);
                auto val = JS::Value(argument);
                dbgln("---- Argument {}: {}", i, val.to_string_without_side_effects());
                arguments->indexed_properties().append(val);
            }

            function(*arguments);
            return JS::js_undefined();
            });

        window->define_native_function(realm, key, move(function_object), 0, JS::Attribute::Writable | JS::Attribute::Configurable);
        dbgln("-- Defined function '{}'", name);
    }
}