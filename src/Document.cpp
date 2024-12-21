#include "Forward.h"
#include "Document.h"
#include "Window.h"
#include "MainThreadVM.h"
#include "EnvironmentSettingsObject.h"
#include "LogClient.h"
#include "Script.h"
#include "Value.h"
#include "ExternError.h"

#include <AK/Format.h>
#include <AK/Try.h>
#include <LibGC/Cell.h>
#include <LibGC/Root.h>
#include <LibJS/Bytecode/Interpreter.h>
#include <LibJS/Runtime/VM.h>
#include <LibJS/Runtime/Array.h>
#include <LibJS/Runtime/ConsoleObject.h>
#include <LibJS/Runtime/PropertyKey.h>
#include <LibJS/Runtime/FunctionObject.h>
#include <LibJS/Runtime/AbstractOperations.h>

// DEBUBG: Just testing static root document
static GC::Root<Document> s_document;

GC_DEFINE_ALLOCATOR(Document);

Document::Document(JS::Realm& realm)
    : JS::GlobalObject(realm)
    , m_realm(realm)
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

    // auto document = Document::create(window->realm());
    auto document = main_thread_vm().heap().allocate<Document>(window->realm());
    document->m_window = window;
    window->set_associated_document(document);


    auto& console_object = *window->realm().intrinsics().console_object();
    auto console_client = main_thread_vm().heap().allocate<LogClient>(console_object.console(), document);
    console_object.console().set_client(console_client);;

    // Root level execution context
    // main_thread_vm().push_execution_context(*document->execution_context());
    return document;
}

void Document::visit_edges(GC::Cell::Visitor& visitor) {
    Base::visit_edges(visitor);
    visitor.visit(m_window);
    visitor.visit(m_realm);
    visitor.visit(m_last_value);
    visitor.visit(m_current_script);
    // visitor.visit(m_console_object);
}

bool Document::log(JS::Console::LogLevel log_level, StringView content)
{
    if (m_on_console_log_ptr)
    {

        m_on_console_log_ptr(log_level, content.characters_without_null_termination(), content.length());
        return true;
    }
    return false;
}

ErrorOr<JS::Value> Document::evaluate(StringView source, StringView source_name)
{
    dbgln("-- Parsing and running '{}'", source_name);

    auto& realm = window()->realm();
    auto& vm = realm.vm();

    // auto& console_object = *realm.intrinsics().console_object();
    // LogClient console_client(console_object.console(), *this);
    // console_object.console().set_client(console_client);

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
        prepare_to_run_script(realm);
        auto strval = MUST(thrown_value.to_string(vm));
        clean_up_after_running_script(realm);

        if (!thrown_value.is_object() || !is<JS::Error>(thrown_value.as_object()))
        {
            log(JS::Console::LogLevel::Error, strval);
            return {};
        }

        auto stack_trace = static_cast<JS::Error const&>(thrown_value.as_object()).stack_string(JS::CompactTraceback::Yes);
        auto formatted = TRY(AK::String::formatted("Uncaught {}\n{}", strval, stack_trace));
        log(JS::Console::LogLevel::Error, formatted);
        return {};
        };


    if (result.is_error()) {
        VERIFY(result.throw_completion().value().has_value());
        auto thrown_value = *result.release_error().value();
        TRY(handle_exception(thrown_value));
        return thrown_value;
    }

    return result.value();
}

GC::Ptr<Script> Document::load_script(StringView source, StringView source_name)
{
    auto& realm = window()->realm();
    auto script = Script::create(source_name.to_byte_string(), source, realm);
    m_current_script = move(script);
    return script;
}

extern "C" {
    Document* document_create() {
        auto document = Document::create_and_initialize();
        s_document = GC::make_root(document);
        return document.ptr();
    }

    EncodedValue document_evaluate(Document* document, const char* source, const char* source_name) {
        auto run_or_errored = document->evaluate(StringView{ source, strlen(source) }, StringView{ source_name, strlen(source_name) });
        if (run_or_errored.is_error()) {
            warnln("Failed to evaluate script: {}", run_or_errored.error().string_literal());
            return encode_js_value(JS::js_undefined());
        }

        document->m_last_value = run_or_errored.value();
        return encode_js_value(document->m_last_value);
    }

    void document_set_on_console_log(Document* document, void (*on_console_log)(JS::Console::LogLevel, const char*, int)) {
        document->set_on_console_log(Function<void(JS::Console::LogLevel, const char*, int)>(on_console_log));
    }

    void document_define_function(Document* document, const char* name, EncodedValue(*function)(EncodedValue)) {
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

            JS::Value arguments_value = JS::Value(arguments);
            EncodedValue encodedResult = function(encode_js_value(arguments_value));
            auto value = decode_js_value(encodedResult);

            // ExternErrors throw
            if (value.is_object() && is<ExternError>(value.as_object())) {

                auto& error = static_cast<ExternError const&>(value.as_object());
                return JS::Completion{ JS::Completion::Type::Throw, value };
            }

            return value;
            });

        window->define_native_function(realm, key, move(function_object), 0, JS::Attribute::Writable | JS::Attribute::Configurable);
    }
}