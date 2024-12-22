#include "Forward.h"
#include "LogClient.h"
#include "MainThreadVM.h"
#include <AK/String.h>
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
    StringBuilder builder;

    if (log_level == JS::Console::LogLevel::Trace)
    {
        auto trace = arguments.get<JS::Console::Trace>();
        if (!trace.label.is_empty())
            builder.appendff("{}\033[36;1m{}\033[0m\n", indent, trace.label);

        for (auto& function_name : trace.stack)
            builder.appendff("{}-> {}\n", indent, function_name);
    }
    else if (log_level == JS::Console::LogLevel::Group || log_level == JS::Console::LogLevel::GroupCollapsed) {
        auto group = arguments.get<JS::Console::Group>();
        builder.appendff("{}\033[36;1m{}\033[0m", indent, group.label);
        m_group_stack_depth++;
    }
    else
    {
        auto output = TRY(generically_format_values(arguments.get<GC::MarkedVector<JS::Value>>()));
        switch (log_level) {
        case JS::Console::LogLevel::Debug:
            builder.appendff("{}\033[36;1m{}\033[0m", indent, output);
            break;
        case JS::Console::LogLevel::Error:
        case JS::Console::LogLevel::Assert:
            builder.appendff("{}\033[31;1m{}\033[0m", indent, output);
            break;
        case JS::Console::LogLevel::Info:
            builder.appendff("{}(i) {}", indent, output);
            break;
        case JS::Console::LogLevel::Log:
            builder.appendff("{}{}", indent, output);
            break;
        case JS::Console::LogLevel::Warn:
        case JS::Console::LogLevel::CountReset:
            builder.appendff("{}\033[33;1m{}\033[0m", indent, output);
            break;
        default:
            builder.appendff("{}{}", indent, output);
            break;
        }
    }

    if (!environment()->log(log_level, builder.string_view())) {
        outln("{}", builder.string_view());
    }

    return JS::js_undefined();
}
