#ifndef ALGINE_ANDROID_H
#define ALGINE_ANDROID_H

#define alSTRINGIFY2(X) #X
#define alSTRINGIFY(X) alSTRINGIFY2(X)

#define __algine_application_entry_point __algine_application_initialization
#define __algine_application_entry_point_str alSTRINGIFY(__algine_application_entry_point)

#define AlgineAppInit extern "C" void __algine_application_entry_point()

#endif //ALGINE_ANDROID_H
