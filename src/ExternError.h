#pragma once

#include <LibJS/Runtime/Error.h>
#include <LibJS/Runtime/Realm.h>
#include <AK/String.h>

class ExternError : public JS::Error {
    JS_OBJECT(ExternError, JS::Error);
    GC_DECLARE_ALLOCATOR(ExternError);

public:
    static GC::Ref<ExternError> create(JS::Realm&);
    static GC::Ref<ExternError> create(JS::Realm&, String message);
    static GC::Ref<ExternError> create(JS::Realm&, StringView message);
    static GC::Ref<ExternError> create(JS::Realm&, StringView message, StringView stack_trace);

    explicit ExternError(Object& prototype);
    virtual ~ExternError() override = default;

    virtual AK::String stack_string(JS::CompactTraceback compact = JS::CompactTraceback::No) const override;

private:
    AK::String m_stack_trace;
};
