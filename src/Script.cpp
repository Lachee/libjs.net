#include "Script.h"
#include "Forward.h"
#include "PrincipalHostDefined.h"
#include <LibJS/Script.h>
#include <LibJS/Runtime/Realm.h>
#include <LibJS/Bytecode/Interpreter.h>

GC_DEFINE_ALLOCATOR(Script);

Script::Script(ByteString filename, JS::Realm& realm)
    : m_filename(move(filename))
    , m_realm(realm)
{
}

Script::~Script() = default;

void Script::visit_edges(Cell::Visitor& visitor)
{
    Base::visit_edges(visitor);
    visitor.visit(m_realm);
    visitor.visit(m_parse_error);
    visitor.visit(m_error_to_rethrow);
    visitor.visit(m_script_record);
}

// Not sure why this is required - Lachee
void Script::visit_host_defined_self(JS::Cell::Visitor& visitor)
{
    visitor.visit(*this);
}


// https://html.spec.whatwg.org/multipage/webappapis.html#creating-a-classic-script
// https://whatpr.org/html/9893/webappapis.html#creating-a-classic-script
GC::Ref<Script> Script::create(ByteString filename, StringView source, JS::Realm& realm, size_t source_line_number)
{
    auto& vm = realm.vm();

    // 1. If muted errors is true, then set baseURL to about:blank.
    //  > Not Applicable

    // 2. If scripting is disabled for realm, then set source to the empty string.
    //  > Not Applicable

    // 3. Let script be a new classic script that this algorithm will subsequently initialize.
    // 4. Set script's realm to realm.
    // 5. Set script's base URL to baseURL.
    auto script = vm.heap().allocate<Script>(move(filename), realm);

    // FIXME: 6. Set script's fetch options to options.

    // 7. Set script's muted errors to muted errors.
    //  > Not Applicable

    // 8. Set script's parse error and error to rethrow to null.
    script->set_parse_error(JS::js_null());
    script->set_error_to_rethrow(JS::js_null());

    // FIXME: 9. Record classic script creation time given script and sourceURLForWindowScripts .

    // 10. Let result be ParseScript(source, realm, script).
    auto result = JS::Script::parse(source, realm, script->filename(), script, source_line_number);

    // 11. If result is a list of errors, then:
    if (result.is_error()) {
        auto& parse_error = result.error().first();
        warnln("Script: Failed to parse: {}", parse_error.to_string());

        // 1. Set script's parse error and its error to rethrow to result[0].
        script->set_parse_error(JS::SyntaxError::create(realm, parse_error.to_string()));
        script->set_error_to_rethrow(script->parse_error());

        // 2. Return script.
        return script;
    }

    // 12. Set script's record to result.
    script->m_script_record = *result.release_value();

    // 13. Return script.
    return script;
}

// https://html.spec.whatwg.org/multipage/webappapis.html#run-a-classic-script
// https://whatpr.org/html/9893/webappapis.html#run-a-classic-script
JS::Completion Script::run(RethrowErrors rethrow_errors, GC::Ptr<JS::Environment> lexical_environment_override)
{
    // 1. Let realm be the realm of script.
    auto& realm = this->realm();

    // 2. Check if we can run script with realm. If this returns "do not run" then return NormalCompletion(empty).
    // > Not Applicable

    // 3. Prepare to run script given realm.
    prepare_to_run_script(realm);

    // 4. Let evaluationStatus be null.
    JS::Completion evaluation_status;

    // 5. If script's error to rethrow is not null, then set evaluationStatus to Completion { [[Type]]: throw, [[Value]]: script's error to rethrow, [[Target]]: empty }.
    if (!error_to_rethrow().is_null()) {
        evaluation_status = JS::Completion{ JS::Completion::Type::Throw, error_to_rethrow() };
    }
    else {
        // 6. Otherwise, set evaluationStatus to ScriptEvaluation(script's record).
        evaluation_status = vm().bytecode_interpreter().run(*m_script_record, lexical_environment_override);
    }

    // 7. If evaluationStatus is an abrupt completion, then:
    if (evaluation_status.is_abrupt()) {
        // 1. If rethrow errors is true and script's muted errors is false, then:
        if (rethrow_errors == RethrowErrors::Yes) {
            // 1. Clean up after running script with realm.
            clean_up_after_running_script(realm);

            // 2. Rethrow evaluationStatus.[[Value]].
            return JS::throw_completion(*evaluation_status.value());
        }

        // 3. Otherwise, rethrow errors is false. Perform the following steps:
        VERIFY(rethrow_errors == RethrowErrors::No);

        // 1. Report an exception given by evaluationStatus.[[Value]] for realms's global object.
        // auto* window_or_worker = dynamic_cast<WindowOrWorkerGlobalScopeMixin*>(&realm.global_object());
        // VERIFY(window_or_worker);
        // window_or_worker->report_an_exception(*evaluation_status.value());
        // > Not Applicable

        // 2. Clean up after running script with realm.
        clean_up_after_running_script(realm);

        // 3. Return evaluationStatus.
        return evaluation_status;
    }

    // 8. Clean up after running script with realm.
    clean_up_after_running_script(realm);

    // 9. If evaluationStatus is a normal completion, then return evaluationStatus.
    VERIFY(!evaluation_status.is_abrupt());
    return evaluation_status;

    // FIXME: 10. If we've reached this point, evaluationStatus was left as null because the script was aborted prematurely during evaluation.
    //            Return Completion { [[Type]]: throw, [[Value]]: a new "QuotaExceededError" DOMException, [[Target]]: empty }.
}

// EnvironmentSettingsObject& Script::settings_object()
// {
//     // The settings object of a script is the settings object of the principal realm of the script's realm.
//     return *verify_cast<PrincipalHostDefined>(realm.host_defined())->environment_settings_object;
// }
