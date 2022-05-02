#ifndef ALGINE_GL_H
#define ALGINE_GL_H

#ifdef __ANDROID__
    // Include the latest possible header file( GL version header )
    #if __ANDROID_API__ >= 24
        #include <GLES3/gl32.h>
    #elif __ANDROID_API__ >= 21
        #include <GLES3/gl31.h>
    #else
        #include <GLES3/gl3.h>
    #endif
#else
    #if defined(__unix__) && !defined(GLEW_NO_GLU)
        #define GLEW_NO_GLU
    #endif

    #include <GL/glew.h>
#endif

#endif //ALGINE_GL_H
