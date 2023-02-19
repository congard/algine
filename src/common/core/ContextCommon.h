#ifndef ALGINE_TESTS_CONTEXTCOMMON_H
#define ALGINE_TESTS_CONTEXTCOMMON_H

#include <algine/core/Engine.h>
#include <algine/core/log/Log.h>

#include "core/debug/Debug.h"

#include <thread>

#ifdef ALGINE_SECURE_OPERATIONS
    #define sop_context_destroyed(context) Engine::contextDestroyed(context)
#else
    #define sop_context_destroyed(context)
#endif

namespace algine {
inline void printContextCreationErrorMessage() {
    Log::error("Context::create", "Context creation failed");
}

inline void printContextNotInitializedError() {
    Log::error("Context", "Context has not been initialized yet");
}

inline void enableDebugOutputIfPossible() {
    if (auto &debugWriter = Engine::getDebugWriter(); debugWriter != nullptr) {
        debugWriter->logger() << "Context " << Context::getCurrentNative() << " now is current for thread "
            << std::hash<std::thread::id>{}(std::this_thread::get_id());

        enableDebugOutput();
    }
}
}

#endif //ALGINE_TESTS_CONTEXTCOMMON_H
