#ifndef ALGINE_SOP_H
#define ALGINE_SOP_H

/* Must be defined next macros if ALGINE_SECURE_OPERATIONS defined:
 *  SOP_BOUND_PTR - Engine's member (m_boundFramebuffer etc)
 *  SOP_OBJECT_TYPE - enum from SOPConstants.h
 *  SOP_OBJECT_ID - id (of Framebuffer, Texture2D etc)
 *  SOP_OBJECT_NAME - string from SOPConstants.h
 */

#ifdef ALGINE_SECURE_OPERATIONS
    #define v_cast(ptr) (void*)(ptr)

    #define sop_string_error_message() \
        std::string message = "Non-bonded "; \
        message += SOP_OBJECT_NAME; \
        message += " (id " + std::to_string(SOP_OBJECT_ID) + ") state change\n"; \

    #if ALGINE_SOP_LEVEL == 0
        #include <algine/core/log/Log.h>

        #define checkBinding() \
            if (v_cast(SOP_BOUND_PTR) != v_cast(this)) { \
                sop_string_error_message() \
                Log::error("AlgineSOP") << message; \
            }
    #elif ALGINE_SOP_LEVEL == 1
        #include <stdexcept>

        #define checkBinding() \
            if (v_cast(SOP_BOUND_PTR) != v_cast(this)) { \
                sop_string_error_message() \
                throw std::runtime_error(message); \
            }
    #else
        #error "Unknown SOP level"
    #endif

    #define commitBinding() Engine::setBoundObject(SOP_OBJECT_TYPE, this);
    #define commitUnbinding() Engine::setBoundObject(SOP_OBJECT_TYPE, nullptr);
#else
    #define checkBinding()
    #define commitBinding()
    #define commitUnbinding()
#endif

#endif //ALGINE_SOP_H
