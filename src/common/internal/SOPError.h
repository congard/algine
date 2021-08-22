#ifndef ALGINE_SOPERROR_H
#define ALGINE_SOPERROR_H

#ifdef ALGINE_SECURE_OPERATIONS
    #if ALGINE_SOP_LEVEL == 0
        #include <algine/core/log/Log.h>

        #define ALGINE_SOP_ERROR(message) Log::error("AlgineSOP") << message << Log::end
    #elif ALGINE_SOP_LEVEL == 1
        #include <stdexcept>

        #define ALGINE_SOP_ERROR(message) throw std::runtime_error(message)
    #else
        #error "Unknown SOP level"
    #endif
#endif

#endif //ALGINE_SOPERROR_H
