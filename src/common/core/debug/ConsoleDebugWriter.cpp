#include <algine/core/debug/ConsoleDebugWriter.h>
#include <algine/core/log/Log.h>

namespace algine {
Logger ConsoleDebugWriter::logger() {
    Logger logger;
    logger.setTag("ConsoleDebugLogger");
    logger.setInputEndListener(this);
    return logger;
}

void ConsoleDebugWriter::onInputEnd(Logger &logger) {
    Log::info(logger.getTag(), logger.str());
}
}
