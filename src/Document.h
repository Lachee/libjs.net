#pragma once

#include "Forward.h"
#include "Window.h"
#include "Script.h"
#include <AK/Function.h>
#include <LibGC/Cell.h>
#include <LibGC/Root.h>
#include <LibJS/Console.h>

class Document final : public JS::GlobalObject {
    JS_OBJECT(Document, JS::GlobalObject);
    GC_DECLARE_ALLOCATOR(Document);

private:
    GC::Ptr<GameWindow> m_window;
    GC::Ptr<JS::Realm> m_realm;
    Function<void(JS::Console::LogLevel, const char*, int)> m_on_console_log_ptr{ nullptr };

protected:
    Document(JS::Realm&);
    virtual void initialize(JS::Realm&) override;
    virtual void visit_edges(GC::Cell::Visitor&) override;

public:
    virtual ~Document() override;

    [[nodiscard]] static  GC::Ref<Document> create(JS::Realm& realm);
    static GC::Ref<Document> create_and_initialize();

    GC::Ptr<GameWindow> window() { return m_window; }
    GC::Ptr<JS::Realm> realm() { return m_realm; }

    bool log(JS::Console::LogLevel, StringView content);
    void set_on_console_log(Function<void(JS::Console::LogLevel, const char*, int)> on_console_log) {
        m_on_console_log_ptr = move(on_console_log);
    }

    ErrorOr<JS::Value> evaluate(StringView source, StringView source_name);

    GC::Ptr<Script> load_script(StringView source, StringView source_name);

    GC::Ptr<Script> m_current_script;
    JS::Value m_last_value;
};


extern "C" {
    Document* document_create();

    EncodedValue document_evaluate(Document* document, const char* source, const char* source_name);
    void document_set_on_console_log(Document* document, void (*on_console_log)(JS::Console::LogLevel, const char*, int));
    void document_define_function(Document* document, const char* name, EncodedValue(*function)(EncodedValue));
}
