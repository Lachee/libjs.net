#pragma once

#include "Forward.h"
#include "Window.h"

class Environment final : public JS::GlobalObject {
    JS_OBJECT(Environment, JS::GlobalObject);

private:
    GC::Ptr<GameWindow> m_window;

private:
    Environment(JS::Realm&);
    virtual void initialize(JS::Realm&) override;

public:
    virtual ~Environment() override;

    [[nodiscard]] static  GC::Ref<Environment> create(JS::Realm& realm);
    static GC::Ref<Environment> create_and_initialize();

    GC::Ptr<GameWindow> window() { return m_window; }

    ErrorOr<bool> parse_and_run(StringView source, StringView source_name);
};


extern "C" {
    Environment* extern_create_environment();
    bool extern_parse_and_run(Environment* enviornment, const char* source, const char* source_name);
}
