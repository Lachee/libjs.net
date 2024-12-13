#pragma once

#include "Forward.h"
#include "Environment.h"
#include <LibGC/Cell.h>
#include <LibJS/Console.h>

class LogClient final : public JS::ConsoleClient {
    GC_CELL(LogClient, JS::ConsoleClient);

public:
    LogClient(JS::Console& console, Environment& environment)
        : ConsoleClient(console)
        , m_environment(environment) {
    }

    virtual void clear() override;
    virtual void end_group() override;

    // 2.3. Printer(logLevel, args[, options]), https://console.spec.whatwg.org/#printer
    virtual JS::ThrowCompletionOr<JS::Value> printer(JS::Console::LogLevel log_level, PrinterArguments arguments) override;

    GC::Ref<Environment> environment() const { return m_environment; }

private:
    GC::Ref<Environment> m_environment;
    int m_group_stack_depth{ 0 };

};