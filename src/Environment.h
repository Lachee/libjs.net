#pragma once

#include <LibJS/Runtime/Object.h>
#include <LibJS/Runtime/GlobalObject.h>

class GlobalThis final : public JS::GlobalObject {
    JS_OBJECT(GlobalThis, JS::GlobalObject);
public:
  GlobalThis(JS::Realm &realm) : JS::GlobalObject(realm) {
  }

  virtual void initialize(JS::Realm &) override;
  virtual ~GlobalThis() override = default;

private:
    JS_DECLARE_NATIVE_FUNCTION(invoke);

};

class Environment
{
public:
    void* m_invoke_function;

private:
#if 0
    JS::VM *m_vm;
    NeverDestroyed<RefPtr<JS::VM>> m_vm_storage;
    OwnPtr<JS::ExecutionContext> m_execution_context;
#endif 

public:
    Environment() {}
    ~Environment(){}

#if 0
    GC::Ptr<JS::Realm> realm() {
        return this->m_execution_context->realm;
    }

    GlobalThis& global_object() {
        return static_cast<GlobalThis&>(this->m_execution_context->realm->global_object());
    }

    bool initialize_vm();
    JS::ThrowCompletionOr<JS::Value> execute(StringView source, StringView source_name);
#endif
};

struct WebEngineCustomData final : public JS::VM::CustomData {

    virtual ~WebEngineCustomData() override = default;
    

    virtual void spin_event_loop_until(GC::Root<GC::Function<bool()>> goal_condition) override;

    Environment *container;

    // GC::Root<HTML::EventLoop> event_loop;

    // GC::Root<JS::Realm> internal_realm;
    // OwnPtr<JS::ExecutionContext> root_execution_context;
};

extern "C" {
    Environment* create_environment();
    void set_invoke(Environment *vm, void *function_ptr);
    void run(Environment *vm, const char *source);
    // void set_function(VMContainer *vm, const char *name, void *function_ptr);
}
