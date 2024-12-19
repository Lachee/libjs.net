#pragma once
#include "Forward.h"
#include <LibJS/Runtime/Environment.h>
#include <LibJS/Runtime/ExecutionContext.h>
#include <LibJS/Runtime/Realm.h>

class EnvironmentSettingsObject : public JS::Cell {
    GC_CELL(EnvironmentSettingsObject, JS::Cell);
    GC_DECLARE_ALLOCATOR(EnvironmentSettingsObject);

public:
    String id;

    static void setup(NonnullOwnPtr<JS::ExecutionContext>);

    virtual ~EnvironmentSettingsObject() override;
    virtual void initialize(JS::Realm&) override;

    // https://html.spec.whatwg.org/multipage/webappapis.html#concept-environment-target-browsing-context
    JS::ExecutionContext& realm_execution_context();
    JS::ExecutionContext const& realm_execution_context() const;

    JS::Realm& realm();
    JS::Object& global_object();

    GC::Ptr<Document> responsible_document();

protected:
    explicit EnvironmentSettingsObject(NonnullOwnPtr<JS::ExecutionContext>);

private:

    EnvironmentSettingsObject(GameWindow&, NonnullOwnPtr<JS::ExecutionContext>);

    NonnullOwnPtr<JS::ExecutionContext> m_realm_execution_context;
    GC::Ptr<GameWindow> m_window;

    virtual void visit_edges(JS::Cell::Visitor&) override;
};

void prepare_to_run_script(JS::Realm&);
void clean_up_after_running_script(JS::Realm const&);

JS::ExecutionContext const& execution_context_of_realm(JS::Realm const&);
inline JS::ExecutionContext& execution_context_of_realm(JS::Realm& realm) { return const_cast<JS::ExecutionContext&>(execution_context_of_realm(const_cast<JS::Realm const&>(realm))); }
