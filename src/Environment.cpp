#include "Environment.h"

#include <AK/String.h>
#include "LibJS/Runtime/Object.h"
#include <LibJS/Bytecode/Interpreter.h>


bool Environment::initialize_vm() {
  // VM initialization implementation
  auto vm_or_error =  JS::VM::create(make<WebEngineCustomData>());
  if (vm_or_error.is_error()) 
    return false;

  auto vm = vm_or_error.value();
  this->m_vm_storage.get() = vm;
  this->m_vm = this->m_vm_storage->ptr();
  this->m_vm->heap().set_should_collect_on_every_allocation(true);
  this->m_vm->set_dynamic_imports_allowed(false);
  this->m_execution_context = JS::create_simple_execution_context<GlobalThis>(*this->m_vm);

  auto& custom_data = verify_cast<WebEngineCustomData>(*vm->custom_data());
  custom_data.container = this;
  // custom_data.event_loop = s_main_thread_vm->heap().allocate<HTML::EventLoop>(type);
  // custom_data.root_execution_context = move(this->m_execution_context);
  // custom_data.internal_realm = 
  return true;
}

JS::ThrowCompletionOr<JS::Value> Environment::execute(StringView source, StringView source_name) {
  auto &vm = *this->m_vm;
  auto &realm = *this->m_execution_context->realm;

  JS::ThrowCompletionOr<JS::Value> result{JS::js_undefined()};

  // Compile and run the script
  auto script_or_error = JS::Script::parse(source, realm, source_name);
  if (script_or_error.is_error()) {
    auto error = script_or_error.error()[0];
    auto hint = error.source_location_hint(source);
    if (!hint.is_empty()) outln("{}", hint);

    auto error_string = error.to_string();
    outln("{}", error_string);
    result = vm.throw_completion<JS::SyntaxError>(move(error_string));
  } else {
    auto script_or_module = script_or_error.value();
    result = vm.bytecode_interpreter().run(*script_or_module);
  }

  return result;
}

extern "C" {  
Environment *create_environment() {
  auto vm_container = new Environment();
  if (!vm_container->initialize_vm())
    return nullptr;
  return vm_container;
}

void set_invoke(Environment *vm, void *function_ptr) {
  vm->m_invoke_function = function_ptr;
}

void run(Environment *vm, const char *source) {
  auto source_sv = StringView{ source, strlen(source) };
  auto result = vm->execute(source_sv, "run"sv);
}

// void set_function(VMContainer *vm, const char *name, void *function_ptr) {
  // StringView view = StringView{name, strlen(name)};
  // auto function_name = view.hash();
  // function_map.set(function_name, reinterpret_cast<void (*)(void)>(function_ptr));
// }
}


void GlobalThis::initialize(JS::Realm &realm) {
  Base::initialize(realm);

  define_direct_property("global", this, JS::Attribute::Enumerable);

  u8 attr = JS::Attribute::Configurable | JS::Attribute::Writable | JS::Attribute::Enumerable;
  // define_native_function(realm, "print", print, 1, attr);
  // define_native_function(realm, "invoke", invoke, 1, attr);

}


JS_DEFINE_NATIVE_FUNCTION(GlobalThis::invoke) {
  JS::Value name = vm.argument(0);
  if (!name.is_string())
    return vm.throw_completion<JS::TypeError>(JS::ErrorType::NotAString);

  auto container = ((WebEngineCustomData *)(vm.custom_data()))->container;
  reinterpret_cast<void(*)(void)>(container->m_invoke_function)();
  return JS::js_undefined();
}


void WebEngineCustomData::spin_event_loop_until(GC::Root<GC::Function<bool()>> goal_condition)
{
  // TODO: Implement event loop
  // Platform::EventLoopPlugin::the().spin_until(move(goal_condition));
}
