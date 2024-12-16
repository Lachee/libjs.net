#pragma once

#include "Forward.h"
#include "Window.h"
#include <AK/Function.h>
#include <LibJS/Console.h>

class Document final : public JS::GlobalObject {
    JS_OBJECT(Document, JS::GlobalObject);

private:
    GC::Ptr<GameWindow> m_window;
    GC::Ptr<JS::Realm> m_realm;
    AK::OwnPtr<JS::ExecutionContext> m_execution_context;
    Function<void(JS::Console::LogLevel, const char*)> m_on_console_log_ptr{ nullptr };

private:
    Document(JS::Realm&);
    virtual void initialize(JS::Realm&) override;

public:
    virtual ~Document() override;

    [[nodiscard]] static  GC::Ref<Document> create(JS::Realm& realm);
    static GC::Ref<Document> create_and_initialize();

    GC::Ptr<GameWindow> window() { return m_window; }
    GC::Ptr<JS::Realm> realm() { return m_realm; }
    AK::OwnPtr<JS::ExecutionContext>& execution_context() { return m_execution_context; }

    bool log(JS::Console::LogLevel, StringView content);
    void set_on_console_log(Function<void(JS::Console::LogLevel, const char*)> on_console_log) {
        m_on_console_log_ptr = move(on_console_log);
    }

    ErrorOr<JS::Value> evaluate(StringView source, StringView source_name);
};


extern "C" {
    Document* environment_create();
    JS::Value* environmnet_evaluate(Document* enviornment, const char* source, const char* source_name);
    void environment_set_on_console_log(Document* environment, void (*on_console_log)(JS::Console::LogLevel, const char*));
    void environment_define_function(Document* environment, const char* name, void (*function)(JS::Array&));
}
