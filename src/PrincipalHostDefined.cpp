#include "PrincipalHostDefined.h"
#include "Forward.h"
#include "EnvironmentSettingsObject.h"

#include <LibJS/Heap/Cell.h>
#include <LibJS/Runtime/Realm.h>

void PrincipalHostDefined::visit_edges(JS::Cell::Visitor& visitor)
{
    HostDefined::visit_edges(visitor);
    visitor.visit(environment_settings_object);
}

