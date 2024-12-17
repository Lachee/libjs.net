#pragma once

#include "Forward.h"
#include <LibGC/Ptr.h>
#include <LibJS/Runtime/Realm.h>


struct PrincipalHostDefined : public JS::Realm::HostDefined {
    explicit PrincipalHostDefined(GC::Ref<EnvironmentSettingsObject> eso)
        : environment_settings_object(eso)
    {
    }
    virtual ~PrincipalHostDefined() override = default;
    virtual void visit_edges(JS::Cell::Visitor& visitor) override;

    GC::Ref<EnvironmentSettingsObject> environment_settings_object;
};