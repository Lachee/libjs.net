#include "ExternError.h"
#include "Forward.h"
#include <LibJS/Runtime/Error.h>
#include <LibJS/Runtime/Realm.h>

GC_DEFINE_ALLOCATOR(ExternError);
GC::Ref<ExternError> ExternError::create(JS::Realm& realm)
{
    return realm.create<ExternError>(realm.intrinsics().error_prototype());
}

GC::Ref<ExternError> ExternError::create(JS::Realm& realm, String message)
{
    auto& vm = realm.vm();
    auto error = ExternError::create(realm);
    u8 attr = JS::Attribute::Writable | JS::Attribute::Configurable;
    error->define_direct_property(vm.names.message, JS::PrimitiveString::create(vm, move(message)), attr);
    return error;
}

GC::Ref<ExternError> ExternError::create(JS::Realm& realm, StringView message)
{
    return create(realm, MUST(String::from_utf8(message)));
}

ExternError::ExternError(Object& prototype)
    : Error(prototype)
{
}

