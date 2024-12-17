#include "Forward.h"
#include "Document.h"
#include "Window.h"
#include "MainThreadVM.h"
#include "EnvironmentSettingsObject.h"
#include "LogClient.h"

#include <AK/Format.h>
#include <AK/Try.h>
#include <LibJS/Bytecode/Interpreter.h>
#include <LibJS/Runtime/VM.h>
#include <LibJS/Runtime/Array.h>
#include <LibJS/Runtime/ConsoleObject.h>
#include <LibJS/Runtime/PropertyKey.h>

Document::Document(JS::Realm& realm)
    : JS::GlobalObject(realm),
    m_realm(realm)
{
}

Document::~Document() = default;

GC::Ref<Document> Document::create(JS::Realm& realm)
{
    return realm.create<Document>(realm);
}

void Document::initialize(JS::Realm& realm)
{
    Base::initialize(realm);
}

GC::Ref<Document> Document::create_and_initialize()
{
    dbgln("-- Creating and initializing document");

    GC::Ptr<GameWindow> window;

    auto realm_execution_context = create_a_new_javascript_realm(
        [&](JS::Realm& realm) -> JS::Object* {
            window = GameWindow::create(realm);
            return window;
        }, nullptr);


    // 6. Set window to the global object of realmExecutionContext's Realm component.
    // Im sure this is here for a reason. Ladybird does it so I will too - Lake.
    window = verify_cast<GameWindow>(realm_execution_context->realm->global_object());

    EnvironmentSettingsObject::setup(
        move(realm_execution_context)
    );

    auto document = Document::create(window->realm());
    document->m_window = window;

    window->set_associated_document(document);

    // Root level execution context
    // main_thread_vm().push_execution_context(*document->execution_context());
    return document;
}

bool Document::log(JS::Console::LogLevel log_level, StringView content)
{
    if (m_on_console_log_ptr) {
        m_on_console_log_ptr(log_level, content.characters_without_null_termination());
        return true;
    }
    return false;
}

ErrorOr<JS::Value> Document::evaluate(StringView source, StringView source_name)
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
        prepare_to_run_script(realm);
        auto script = script_or_error.value();
        run_script_or_module(script);
        clean_up_after_running_script(realm);
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
    Document* document_create() {
        return Document::create_and_initialize().ptr();
    }

    JS::Value* document_evaluate(Document* document, const char* source, const char* source_name) {
        auto run_or_errored = document->evaluate(StringView{ source, strlen(source) }, StringView{ source_name, strlen(source_name) });
        if (run_or_errored.is_error()) {
            warnln("Failed to evaluate script: {}", run_or_errored.error().string_literal());
            return nullptr;
        }

        return new JS::Value(run_or_errored.value());
    }

    void document_set_on_console_log(Document* document, void (*on_console_log)(JS::Console::LogLevel, const char*)) {
        document->set_on_console_log(Function<void(JS::Console::LogLevel, const char*)>(on_console_log));
    }

    void document_define_function(Document* document, const char* name, void (*function)(JS::Array&)) {
        auto window = document->window();
        auto& realm = window->realm();

        DeprecatedFlyString keyName(name);
        JS::PropertyKey key(keyName);

        auto function_object = AK::Function<JS::ThrowCompletionOr<JS::Value>(JS::VM&)>([function](JS::VM& vm) -> JS::ThrowCompletionOr<JS::Value> {
            auto count = vm.argument_count();
            auto& realm = *vm.current_realm();
            auto arguments = TRY(JS::Array::create(realm, 0));
            for (size_t i = 0; i < count; i++) {
                auto argument = vm.argument(i);
                arguments->indexed_properties().append(argument);
            }

            function(*arguments);
            return JS::js_undefined();
            });

        window->define_native_function(realm, key, move(function_object), 0, JS::Attribute::Writable | JS::Attribute::Configurable);
    }
}