#include "ExternError.h"
#include "Forward.h"
#include <LibJS/Runtime/Error.h>
#include <LibJS/Runtime/Realm.h>
#include <LibJS/SourceRange.h>
#include <LibJS/Runtime/ExecutionContext.h>
#include <AK/String.h>

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

GC::Ref<ExternError> ExternError::create(JS::Realm& realm, StringView message, StringView stack_trace)
{
    auto error = create(realm, MUST(String::from_utf8(message)));
    error->m_stack_trace = MUST(String::from_utf8(stack_trace));
    return error;
}

ExternError::ExternError(Object& prototype)
    : Error(prototype)
{
}

AK::String ExternError::stack_string(JS::CompactTraceback compact) const
{
    auto base_stack = Base::stack_string(compact);
    if (!m_stack_trace.is_empty())
        return MUST(AK::String::formatted("{}\n{}", m_stack_trace, base_stack));
    return base_stack;
}
