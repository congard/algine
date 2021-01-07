#ifndef ALGINE_DEBUG_H
#define ALGINE_DEBUG_H

#define debugGLCall(call) \
{ \
    call; \
    int err = glGetError(); \
    if (err != 0) { \
        std::cerr << "Error " << err << " in " #call "\n\n"; \
    } \
}

#endif //ALGINE_DEBUG_H
