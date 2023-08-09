#ifndef ALGINE_CONTEXTCOMMON_H
#define ALGINE_CONTEXTCOMMON_H

#include <algine/core/log/Log.h>

#include "core/debug/Debug.h"

#include <thread>

namespace algine {
inline void printContextCreationErrorMessage() {
    Log::error("Context::create", "Context creation failed");
}

inline void printContextNotInitializedError() {
    Log::error("Context", "Context has not been initialized yet");
}

inline void enableDebugOutputIfPossible() {
    if (Log::isEnabled(Logger::Type::Debug)) {
        Log::debug("Context") << "Context " << Context::getCurrentNative() << " now is current for thread "
            << std::hash<std::thread::id>{}(std::this_thread::get_id());

        enableDebugOutput();
    }
}
}

#endif //ALGINE_CONTEXTCOMMON_H
