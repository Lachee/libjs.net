#pragma once

#include "Forward.h"
#include "Window.h"
#include <AK/Function.h>
#include <LibJS/Console.h>

class Environment final : public JS::GlobalObject {
    JS_OBJECT(Environment, JS::GlobalObject);

private:
    GC::Ptr<GameWindow> m_window;
    GC::Ptr<JS::Realm> m_realm;
    Function<void(JS::Console::LogLevel, const char*)> m_on_console_log_ptr{ nullptr };

private:
    Environment(JS::Realm&);
    virtual void initialize(JS::Realm&) override;

public:
    virtual ~Environment() override;

    [[nodiscard]] static  GC::Ref<Environment> create(JS::Realm& realm);
    static GC::Ref<Environment> create_and_initialize();

    GC::Ptr<GameWindow> window() { return m_window; }
    GC::Ptr<JS::Realm> realm() { return m_realm; }

    bool log(JS::Console::LogLevel, StringView content);
    void set_on_console_log(Function<void(JS::Console::LogLevel, const char*)> on_console_log) {
        m_on_console_log_ptr = move(on_console_log);
    }

    ErrorOr<JS::Value> evaluate(StringView source, StringView source_name);
};


extern "C" {
    Environment* environment_create();
    JS::Value* environmnet_evaluate(Environment* enviornment, const char* source, const char* source_name);
    void environment_set_on_console_log(Environment* environment, void (*on_console_log)(JS::Console::LogLevel, const char*));
    void environment_define_function(Environment* environment, const char* name, void (*function)(JS::Array&));
}
