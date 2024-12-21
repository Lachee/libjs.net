#pragma once

#include <LibJS/Runtime/Error.h>
#include <LibJS/Runtime/Realm.h>

class ExternError : public JS::Error {
    JS_OBJECT(ExternError, JS::Error);
    GC_DECLARE_ALLOCATOR(ExternError);

public:
    static GC::Ref<ExternError> create(JS::Realm&);
    static GC::Ref<ExternError> create(JS::Realm&, String message);
    static GC::Ref<ExternError> create(JS::Realm&, StringView message);

    explicit ExternError(Object& prototype);
    virtual ~ExternError() override = default;
};
