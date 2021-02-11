#ifndef ALGINE_SOP_H
#define ALGINE_SOP_H

#include "SOPError.h"

/* Must be defined next macros if ALGINE_SECURE_OPERATIONS defined:
 *  SOP_BOUND_PTR - Engine's member (m_boundFramebuffer etc)
 *  SOP_OBJECT_TYPE - enum from SOPConstants.h
 *  SOP_OBJECT_ID - id (of Framebuffer, Texture2D etc)
 *  SOP_OBJECT_NAME - string from SOPConstants.h
 */

#ifdef ALGINE_SECURE_OPERATIONS
    #define v_cast(ptr) (void*)(ptr)

    #define checkBinding() \
        if (v_cast(SOP_BOUND_PTR) != v_cast(this)) { \
            std::string message = "Non-bonded "; \
            message.append(SOP_OBJECT_NAME); \
            message.append(" (id " + std::to_string(SOP_OBJECT_ID) + ") state change\n"); \
            ALGINE_SOP_ERROR(message); \
        }

    #define commitBinding() Engine::setBoundObject(SOP_OBJECT_TYPE, this);
    #define commitUnbinding() Engine::setBoundObject(SOP_OBJECT_TYPE, nullptr);
#else
    #define checkBinding()
    #define commitBinding()
    #define commitUnbinding()
#endif

#endif //ALGINE_SOP_H
