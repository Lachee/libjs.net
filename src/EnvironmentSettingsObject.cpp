#include "Forward.h"
#include "EnvironmentSettingsObject.h"
#include "Window.h"
#include "PrincipalHostDefined.h"
#include <LibJS/Runtime/Environment.h>
#include <LibJS/Runtime/ExecutionContext.h>
#include <LibJS/Runtime/Realm.h>

GC_DEFINE_ALLOCATOR(EnvironmentSettingsObject);

EnvironmentSettingsObject::EnvironmentSettingsObject(GameWindow& window, NonnullOwnPtr<JS::ExecutionContext> execution_context)
    : m_realm_execution_context(move(execution_context))
    , m_window(window)
{
    m_realm_execution_context->context_owner = this;
}

EnvironmentSettingsObject::~EnvironmentSettingsObject() = default;

void EnvironmentSettingsObject::visit_edges(JS::Cell::Visitor& visitor)
{
    Base::visit_edges(visitor);
    m_realm_execution_context->visit_edges(visitor);
    visitor.visit(m_window);
}

void EnvironmentSettingsObject::setup(NonnullOwnPtr<JS::ExecutionContext> execution_context)
{
    // 1. Let realm be the value of execution context's Realm component.
    auto realm = execution_context->realm;
    VERIFY(realm);

    // 2. Let window be realm's global object.
    auto& window = verify_cast<GameWindow>(realm->global_object());

    // 3. Let settings object be a new environment settings object whose algorithms are defined as follows:
    // NOTE: See the functions defined for this class.
    auto settings_object = realm->create<EnvironmentSettingsObject>(window, move(execution_context));

    static i64 next_id = 1;
    settings_object->id = String::number(next_id++);

    // 7. Set realm's [[HostDefined]] field to settings object.
    auto host_defined = make<PrincipalHostDefined>(settings_object);
    realm->set_host_defined(move(host_defined));
}

void EnvironmentSettingsObject::initialize(JS::Realm& realm)
{
    Base::initialize(realm);
}

JS::ExecutionContext& EnvironmentSettingsObject::realm_execution_context()
{
    // NOTE: All environment settings objects are created with a realm execution context, so it's stored and returned here in the base class.
    return *m_realm_execution_context;
}

JS::ExecutionContext const& EnvironmentSettingsObject::realm_execution_context() const
{
    // NOTE: All environment settings objects are created with a realm execution context, so it's stored and returned here in the base class.
    return *m_realm_execution_context;
}

JS::Realm& EnvironmentSettingsObject::realm()
{
    // An environment settings object's realm execution context's Realm component is the environment settings object's Realm.
    return *realm_execution_context().realm;
}

JS::Object& EnvironmentSettingsObject::global_object()
{
    // An environment settings object's Realm then has a [[GlobalObject]] field, which contains the environment settings object's global object.
    return realm().global_object();
}

// https://whatpr.org/html/9893/b8ea975...df5706b/webappapis.html#concept-realm-execution-context
JS::ExecutionContext const& execution_context_of_realm(JS::Realm const& realm)
{
    VERIFY(realm.host_defined());
    return static_cast<PrincipalHostDefined const&>(*realm.host_defined()).environment_settings_object->realm_execution_context();
}

void prepare_to_run_script(JS::Realm& realm)
{
    // 1. Push realms's execution context onto the JavaScript execution context stack; it is now the running JavaScript execution context.
    realm.global_object().vm().push_execution_context(execution_context_of_realm(realm));

    // FIXME: 2. If realm is a principal realm, then:
    // FIXME: 2.1 Let settings be realm's settings object.
    // FIXME: 2.2 Add settings to the currently running task's script evaluation environment settings object set.
}

void clean_up_after_running_script(JS::Realm const& realm)
{
    auto& vm = realm.global_object().vm();

    // 1. Assert: realm's execution context is the running JavaScript execution context.
    VERIFY(&execution_context_of_realm(realm) == &vm.running_execution_context());

    // 2. Remove realm's execution context from the JavaScript execution context stack.
    vm.pop_execution_context();

    // 3. If the JavaScript execution context stack is now empty, perform a microtask checkpoint. (If this runs scripts, these algorithms will be invoked reentrantly.)
    // if (vm.execution_context_stack().is_empty())
        // main_thread_event_loop().perform_a_microtask_checkpoint();
}
