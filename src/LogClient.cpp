#include "Forward.h"
#include "LogClient.h"
#include "MainThreadVM.h"
#include <AK/StringBuilder.h>
#include <LibGC/Cell.h>
#include <LibJS/Console.h>
#include <LibJS/Print.h>


void LogClient::clear()
{
    out("\033[3J\033[H\033[2J");
    m_group_stack_depth = 0;
    fflush(stdout);
}

void LogClient::end_group()
{
    if (m_group_stack_depth > 0)
        m_group_stack_depth--;
}

// 2.3. Printer(logLevel, args[, options]), https://console.spec.whatwg.org/#printer
JS::ThrowCompletionOr<JS::Value> LogClient::printer(JS::Console::LogLevel log_level, PrinterArguments arguments)
{
    auto& g_vm = main_thread_vm();
    auto indent = MUST(String::repeated(' ', m_group_stack_depth * 2));

    if (log_level == JS::Console::LogLevel::Trace) {
        auto trace = arguments.get<JS::Console::Trace>();
        StringBuilder builder;
        if (!trace.label.is_empty())
            builder.appendff("{}\033[36;1m{}\033[0m\n", indent, trace.label);

        for (auto& function_name : trace.stack)
            builder.appendff("{}-> {}\n", indent, function_name);

        outln("{}", builder.string_view());
        return JS::js_undefined();
    }

    if (log_level == JS::Console::LogLevel::Group || log_level == JS::Console::LogLevel::GroupCollapsed) {
        auto group = arguments.get<JS::Console::Group>();
        outln("{}\033[36;1m{}\033[0m", indent, group.label);
        m_group_stack_depth++;
        return JS::js_undefined();
    }

    auto output = TRY(generically_format_values(arguments.get<GC::MarkedVector<JS::Value>>()));
    if (environment()->log(log_level, output))
        return JS::js_undefined();

    switch (log_level) {
    case JS::Console::LogLevel::Debug:
        outln("{}\033[36;1m{}\033[0m", indent, output);
        break;
    case JS::Console::LogLevel::Error:
    case JS::Console::LogLevel::Assert:
        outln("{}\033[31;1m{}\033[0m", indent, output);
        break;
    case JS::Console::LogLevel::Info:
        outln("{}(i) {}", indent, output);
        break;
    case JS::Console::LogLevel::Log:
        outln("{}{}", indent, output);
        break;
    case JS::Console::LogLevel::Warn:
    case JS::Console::LogLevel::CountReset:
        outln("{}\033[33;1m{}\033[0m", indent, output);
        break;
    default:
        outln("{}{}", indent, output);
        break;
    }

    return JS::js_undefined();
}
