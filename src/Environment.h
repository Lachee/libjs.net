#pragma once

#include "Forward.h"
#include "Window.h"
#include <AK/Function.h>
#include <LibJS/Console.h>

class Environment final : public JS::GlobalObject {
    JS_OBJECT(Environment, JS::GlobalObject);

private:
    GC::Ptr<GameWindow> m_window;
    Function<void(JS::Console::LogLevel, const char*)> m_on_console_log_ptr{ nullptr };

private:
    Environment(JS::Realm&);
    virtual void initialize(JS::Realm&) override;

public:
    virtual ~Environment() override;

    [[nodiscard]] static  GC::Ref<Environment> create(JS::Realm& realm);
    static GC::Ref<Environment> create_and_initialize();

    GC::Ptr<GameWindow> window() { return m_window; }

    bool log(JS::Console::LogLevel, StringView content);
    void set_on_console_log(Function<void(JS::Console::LogLevel, const char*)> on_console_log) {
        m_on_console_log_ptr = move(on_console_log);
    }

    ErrorOr<bool> parse_and_run(StringView source, StringView source_name);
};


extern "C" {
    Environment* extern_create_environment();
    bool extern_parse_and_run(Environment* enviornment, const char* source, const char* source_name);
    void extern_set_on_console_log(Environment* environment, void (*on_console_log)(JS::Console::LogLevel, const char*));
}
