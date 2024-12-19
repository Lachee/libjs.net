
#pragma once
#include "Forward.h"
#include "EnvironmentSettingsObject.h"
#include "PrincipalHostDefined.h"

#include <LibJS/Script.h>
#include <LibJS/Runtime/Realm.h>
#include <LibJS/Runtime/Environment.h>
#include <LibGC/Cell.h>

class Script final
    : public JS::Cell
    , public JS::Script::HostDefined {
    GC_CELL(Script, JS::Cell);
    GC_DECLARE_ALLOCATOR(Script);

public:
    virtual ~Script() override;

    static GC::Ref<Script> create(ByteString filename, StringView source, JS::Realm&, size_t source_line_number = 1);

    ByteString const& filename() const { return m_filename; }

    JS::Script* script_record() { return m_script_record; }
    JS::Script const* script_record() const { return m_script_record; }

    JS::Realm& realm() { return m_realm; }
    // EnvironmentSettingsObject& settings_object();

    [[nodiscard]] JS::Value error_to_rethrow() const { return m_error_to_rethrow; }
    void set_error_to_rethrow(JS::Value value) { m_error_to_rethrow = value; }

    [[nodiscard]] JS::Value parse_error() const { return m_parse_error; }
    void set_parse_error(JS::Value value) { m_parse_error = value; }


    enum class RethrowErrors {
        No,
        Yes,
    };
    JS::Completion run(RethrowErrors = RethrowErrors::No, GC::Ptr<JS::Environment> lexical_environment_override = {});

private:
    Script(ByteString filename, JS::Realm&);

    virtual void visit_edges(GC::Cell::Visitor&) override;

    virtual void visit_host_defined_self(JS::Cell::Visitor&) override;

    GC::Ptr<JS::Script> m_script_record;
    ByteString m_filename;
    GC::Ref<JS::Realm> m_realm;
    JS::Value m_parse_error;
    JS::Value m_error_to_rethrow;

};