#pragma once

#include "Forward.h"
#include <LibGC/Cell.h>
#include <LibJS/Console.h>

class LogClient final : public JS::ConsoleClient {
    GC_CELL(LogClient, JS::ConsoleClient);

public:
    LogClient(JS::Console& console)
        : ConsoleClient(console)
    {
    }

    virtual void clear() override;
    virtual void end_group() override;

    // 2.3. Printer(logLevel, args[, options]), https://console.spec.whatwg.org/#printer
    virtual JS::ThrowCompletionOr<JS::Value> printer(JS::Console::LogLevel log_level, PrinterArguments arguments) override;

private:
    int m_group_stack_depth{ 0 };

};