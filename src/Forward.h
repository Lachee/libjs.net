#pragma once

#include <AK/Format.h>
#include <AK/NonnullOwnPtr.h>
#include <AK/OwnPtr.h>
#include <AK/RefPtr.h>
#include <AK/String.h>
#include <AK/Try.h>
#include <LibJS/Runtime/ExecutionContext.h>
#include <LibJS/Runtime/GlobalObject.h>
#include <LibJS/Runtime/Object.h>
#include <LibJS/Runtime/Realm.h>
#include <LibJS/Runtime/VM.h>

using Value = u64;

class Document;
class EnvironmentSettingsObject;
class ExternError;
class GameWindow;
class InteropValue;
class LogClient;
class Script;
struct PrincipalHostDefined;
