#ifndef ALGINE_SOP_H
#define ALGINE_SOP_H

/* Must be defined next macros if ALGINE_SECURE_OPERATIONS defined:
 *  SOP_BOUND_PTR - Engine's member (m_boundFramebuffer etc)
 *  SOP_OBJECT_TYPE - enum from SOPConstants.h
 *  SOP_OBJECT_ID - id (of Framebuffer, Texture2D etc)
 *  SOP_OBJECT_NAME - string from SOPConstants.h
 */

#ifdef ALGINE_SECURE_OPERATIONS
#   define v_cast(ptr) (void*)(ptr)

#   if ALGINE_SOP_LEVEL == 0
#       include <iostream>

#       define checkBinding() \
            if (v_cast(SOP_BOUND_PTR) != v_cast(this)) { \
                std::cerr << "Warning: non-bonded " << SOP_OBJECT_NAME << " (id " + std::to_string(SOP_OBJECT_ID) + ") state change\n"; \
            }
#   elif ALGINE_SOP_LEVEL == 1
#       include <exception>

#       define checkBinding() \
            if (v_cast(SOP_BOUND_PTR) != v_cast(this)) { \
                std::string message = "Non-bonded "; \
                message += SOP_OBJECT_NAME; \
                message += " (id " + std::to_string(SOP_OBJECT_ID) + ") state change"; \
                throw std::runtime_error(message); \
            }
#   else
#       error "Unknown SOP level"
#   endif

#   define commitBinding() Engine::setBoundObject(SOP_OBJECT_TYPE, this);
#   define commitUnbinding() Engine::setBoundObject(SOP_OBJECT_TYPE, nullptr);
#else
#   define checkBinding()
#   define commitBinding()
#   define commitUnbinding()
#endif

#endif //ALGINE_SOP_H
