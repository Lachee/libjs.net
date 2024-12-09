#include "LibJS.h"
#include "StandardIOStream.h"
#include <iostream>

#include <unordered_map>
#include <string>

#include <unicode/datefmt.h>

#include <AK/Types.h>
#include <AK/Try.h>
#include <AK/Error.h>
#include <AK/HashMap.h>
#include <AK/StringView.h>
#include <AK/NeverDestroyed.h>
#include <AK/RefPtr.h>
#include <AK/NonnullOwnPtr.h>
#include <LibCore/StandardPaths.h>
#include <LibJS/Print.h>
#include <LibJS/Runtime/VM.h>
#include <LibJS/Runtime/GlobalObject.h>
#include <LibJS/Runtime/ConsoleObject.h>
#include <LibJS/Bytecode/Interpreter.h>

NeverDestroyed<RefPtr<JS::VM>> g_vm_storage;
JS::VM *g_vm;
GC::Root<JS::Value> g_last_value = GC::make_root(JS::js_undefined());

HashMap<u32, void (*)(void)> function_map;

StandardIOStream stdout_stream(StreamType::Output);
StandardIOStream stderr_stream(StreamType::Error);

enum class PrintTarget
{
    StandardError,
    StandardOutput,
};

static ErrorOr<void> print(JS::Value value, Stream &stream)
{
    JS::PrintContext print_context{.vm = *g_vm, .stream = stream, .strip_ansi = true};
    return JS::print(value, print_context);
}

static ErrorOr<void> print(JS::Value value, PrintTarget target = PrintTarget::StandardOutput)
{
    auto &stream = target == PrintTarget::StandardError ? stderr_stream : stdout_stream;
    return print(value, stream);
}

static ErrorOr<void> pinvoke(JS::Value value)
{
    VERIFY(value.is_string());
    auto function_name = value.as_string().utf8_string_view().hash();
    auto it = function_map.find(function_name);
    if (it == function_map.end())
        return Error::from_string_literal("Function not found");

    auto function_ptr = it->value;
    function_ptr();
    return {};
}

class Context final : public JS::GlobalObject
{
    JS_OBJECT(Context, JS::GlobalObject);

public:
    Context(JS::Realm &realm)
        : JS::GlobalObject(realm)
    {
    }

    virtual void initialize(JS::Realm &) override;
    virtual ~Context() override = default;

private:
    JS_DECLARE_NATIVE_FUNCTION(print);
    JS_DECLARE_NATIVE_FUNCTION(invoke);
};

void Context::initialize(JS::Realm &realm)
{
    Base::initialize(realm);

    define_direct_property("global", this, JS::Attribute::Enumerable);
    u8 attr = JS::Attribute::Configurable | JS::Attribute::Writable | JS::Attribute::Enumerable;
    define_native_function(realm, "print", print, 1, attr);
    define_native_function(realm, "invoke", invoke, 1, attr);
}

JS_DEFINE_NATIVE_FUNCTION(Context::print)
{
    auto result = ::print(vm.argument(0));
    if (result.is_error())
        return g_vm->throw_completion<JS::InternalError>(TRY_OR_THROW_OOM(*g_vm, String::formatted("Failed to print value: {}", result.error())));

    outln();

    return JS::js_undefined();
}

JS_DEFINE_NATIVE_FUNCTION(Context::invoke)
{
    auto result = ::pinvoke(vm.argument(0));
    if (result.is_error())
        return g_vm->throw_completion<JS::InternalError>(TRY_OR_THROW_OOM(*g_vm, String::formatted("Failed to invoke function: {}", result.error())));

    return JS::js_undefined();
}

static ErrorOr<bool> execute(JS::Realm &realm, StringView source, StringView source_name)
{
    auto &vm = realm.vm();

    JS::ThrowCompletionOr<JS::Value> result{JS::js_undefined()};

    auto run_script_or_module = [&](auto &script_or_module)
    {
        // if (s_dump_ast)
        // script_or_module->parse_node().dump(0);

        result = vm.bytecode_interpreter().run(*script_or_module);
    };

    // Execution
    auto script_or_error = JS::Script::parse(source, realm, source_name);
    if (script_or_error.is_error())
    {
        auto error = script_or_error.error()[0];
        auto hint = error.source_location_hint(source);
        if (!hint.is_empty())
            outln("{}", hint);

        auto error_string = error.to_string();
        outln("{}", error_string);
        result = vm.throw_completion<JS::SyntaxError>(move(error_string));
    }
    else
    {
        run_script_or_module(script_or_error.value());
    }

    // Error Handling
    auto handle_exception = [&](JS::Value thrown_value) -> ErrorOr<void>
    {
        warnln("Uncaught exception: ");
        TRY(print(thrown_value, PrintTarget::StandardError));
        warnln();

        if (!thrown_value.is_object() || !is<JS::Error>(thrown_value.as_object()))
            return {};
        warnln("{}", static_cast<JS::Error const &>(thrown_value.as_object()).stack_string(JS::CompactTraceback::Yes));
        return {};
    };

    if (!result.is_error())
    {
        g_last_value = GC::make_root(result.value());
    }
    else
    {
        VERIFY(result.throw_completion().value().has_value());
        TRY(handle_exception(*result.release_error().value()));
        return false;
    }

    // TRY(print(result.value()));
    return true;
}

ErrorOr<int> try_run_script(const char *data)
{
    auto source_content = TRY(ByteBuffer::copy(data, strlen(data)));
    auto source = StringView{source_content};

    auto vm = TRY(JS::VM::create());
    g_vm_storage.get() = vm;
    g_vm = g_vm_storage->ptr();
    g_vm->set_dynamic_imports_allowed(false);
    g_vm->heap().set_should_collect_on_every_allocation(false);

    auto execution_context = JS::create_simple_execution_context<Context>(*g_vm);
    auto &realm = *execution_context->realm;

    if (!TRY(execute(realm, source, "eval"sv)))
        return 1;

    return 0;
}

extern "C" void register_function(const char *name, void *function)
{
    StringView view = StringView{name, strlen(name)};
    auto function_name = view.hash();
    function_map.set(function_name, reinterpret_cast<void (*)(void)>(function));
}

extern "C" int run_script(const char *data)
{
    auto result_or_error = try_run_script(data);
    if (result_or_error.is_error())
        return -1;
    return result_or_error.value();
}
