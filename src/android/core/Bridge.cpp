#include "Bridge.h"

#include <jni.h>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>

#include <algine/core/Engine.h>
#include <algine/core/Screen.h>
#include <algine/core/ScreenEventHandler.h>

#include <algine/Android.h>

#include <tulz/DynamicLibrary.h>
#include <tulz/assets/AssetManager.h>

using namespace algine;
using namespace tulz;

#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, "AlgineBridge", __VA_ARGS__)

#ifdef ALGINE_VERBOSE
    #define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "AlgineBridge", __VA_ARGS__)
#else
    #define ALOGV(...)
#endif

static JavaVM *jvm = nullptr;
static jclass bridge;
static jobject asset_manager;
static DynamicLibrary application;

template<typename T>
void executeCallback(T callback) {
    JNIEnv *env;

    bool detach = false;

    int getEnvStat = jvm->GetEnv((void **)&env, JNI_VERSION_1_6);

    if (getEnvStat == JNI_EDETACHED) {
        ALOGV("GetEnv: not attached");

        if (jvm->AttachCurrentThread(&env, nullptr) != 0) {
            ALOGV("Failed to attach");
            return;
        }

        detach = true;

        ALOGV("Attached");
    } else if (getEnvStat == JNI_OK) {
        // detach = false;
    } else if (getEnvStat == JNI_EVERSION) {
        ALOGV("GetEnv: version not supported");

        return;
    }

    callback(env);

    if (detach) {
        jvm->DetachCurrentThread();
        ALOGV("Detached");
    }
}

#define callback [](JNIEnv *env)
#define callback_capture [&](JNIEnv *env)

extern "C"
jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    ALOGI("JNI_OnLoad");

    jvm = vm;

    JNIEnv *env;
    jvm->GetEnv((void **) &env, JNI_VERSION_1_6);

    bridge = reinterpret_cast<jclass>(env->NewGlobalRef(
            env->FindClass("com/algine/android/module/Bridge")));

//    auto test = env->FindClass("com/algine/android/module/Bridge");
//    jmethodID mid = env->GetStaticMethodID(test, "getViewDimensions",
//                                           "()[I");

    return JNI_VERSION_1_6;
}

// input calls

extern "C"
JNIEXPORT void JNICALL
Java_com_algine_android_module_Bridge_init(JNIEnv *env,
        jclass clazz, jstring application_library, jobject activity)
{
    ALOGI("Bridge init");

    auto nativeString = env->GetStringUTFChars(application_library, nullptr);

    ALOGI("Application: %s", nativeString);

    application.load(nativeString);

    if (auto error = DynamicLibrary::getError(); error.error) {
        ALOGI("Application %s loading error: %s", nativeString, error.message.c_str());
    } else {
        ALOGI("Application %s has been successfully loaded", nativeString);

        Engine::init();

        { // init assets
            jclass activity_class = env->GetObjectClass(activity);

            jmethodID activity_class_getAssets = env->GetMethodID(activity_class, "getAssets", "()Landroid/content/res/AssetManager;");
            jobject local_asset_manager = env->CallObjectMethod(activity, activity_class_getAssets); // activity.getAssets();
            asset_manager = env->NewGlobalRef(local_asset_manager);

            auto assetManager = AAssetManager_fromJava(env, asset_manager);

            AssetManager::setJavaAssetManager(assetManager);
        }

        // get density & calculate dpi value
        // activity.getResources().getDisplayMetrics().density
        // for more information read https://developer.android.com/reference/android/util/DisplayMetrics#density
        {
            jclass activity_class = env->GetObjectClass(activity);

            jmethodID activity_getResources =
                    env->GetMethodID(activity_class, "getResources", "()Landroid/content/res/Resources;");
            jobject resources = env->CallObjectMethod(activity, activity_getResources); // activity.getResources()
            jclass resources_class = env->GetObjectClass(resources);

            jmethodID resources_getDisplayMetrics =
                    env->GetMethodID(resources_class, "getDisplayMetrics", "()Landroid/util/DisplayMetrics;");
            jobject displayMetrics = env->CallObjectMethod(resources, resources_getDisplayMetrics); // resources.getDisplayMetrics()
            jclass displayMetrics_class = env->GetObjectClass(displayMetrics);

            jfieldID displayMetrics_density = env->GetFieldID(displayMetrics_class, "density", "F");
            jfloat density = env->GetFloatField(displayMetrics, displayMetrics_density);

            Engine::setDPI(density * 160.0f);
        }

        { // get current user's language and country
            jmethodID bridge_getCurrentLocale = env->GetStaticMethodID(bridge, "getCurrentLocale", "()Ljava/util/Locale;");
            jobject locale = env->CallStaticObjectMethod(bridge, bridge_getCurrentLocale);
            jclass locale_class = env->GetObjectClass(locale);

            jmethodID locale_getLanguage = env->GetMethodID(locale_class, "getLanguage", "()Ljava/lang/String;");
            auto language = static_cast<jstring>(env->CallObjectMethod(locale, locale_getLanguage));
            auto language_str = env->GetStringUTFChars(language, nullptr);

            jmethodID locale_getCountry = env->GetMethodID(locale_class, "getCountry", "()Ljava/lang/String;");
            auto country = static_cast<jstring>(env->CallObjectMethod(locale, locale_getCountry));
            auto country_str = env->GetStringUTFChars(country, nullptr);

            Engine::setLanguage(language_str);
            Engine::setCountry(country_str);

            env->ReleaseStringUTFChars(language, language_str);
            env->ReleaseStringUTFChars(country, country_str);
        }

        { // init screen size
            auto dimensions = AndroidBridge::getViewDimensions();

            new Screen(); // create an instance of the Screen
            Screen::__resized(dimensions.x, dimensions.y);
        }

        application.getAddress<void(*)()>(__algine_application_entry_point_str)();
    }

    env->ReleaseStringUTFChars(application_library, nativeString);
}

inline ScreenEventHandler* getEventHandler() {
    // if an app will launched when the screen is off, then init will not be called
    // and instance of the Screen will be nullptr, but onPause will be called
    if (Screen::instance() == nullptr)
        return nullptr;

    return reinterpret_cast<ScreenEventHandler *>(Screen::instance()->getEventHandler());
}

#define event(...) \
    if (auto handler = getEventHandler(); handler != nullptr) { \
        handler->__VA_ARGS__; \
    }

extern "C"
JNIEXPORT void JNICALL
Java_com_algine_android_module_Bridge_pointerDown(JNIEnv *env, jclass clazz, jfloat x, jfloat y,
                                                  jint pointer_id) {
    event(pointerDown(x, y, pointer_id))
}

extern "C"
JNIEXPORT void JNICALL
Java_com_algine_android_module_Bridge_pointerMove(JNIEnv *env, jclass clazz, jfloat x, jfloat y,
                                                  jint pointer_id) {
    event(pointerMove(x, y, pointer_id))
}

extern "C"
JNIEXPORT void JNICALL
Java_com_algine_android_module_Bridge_pointerUp(JNIEnv *env, jclass clazz, jfloat x, jfloat y,
                                                jint pointer_id) {
    event(pointerUp(x, y, pointer_id))
}

extern "C"
JNIEXPORT void JNICALL
Java_com_algine_android_module_Bridge_pointerClick(JNIEnv *env, jclass clazz, jfloat x, jfloat y,
                                                   jint pointer_id) {
    event(pointerClick(x, y, pointer_id))
}

extern "C"
JNIEXPORT void JNICALL
Java_com_algine_android_module_Bridge_surfaceResized(JNIEnv *env, jclass clazz, jint width,
                                                     jint height) {
    Screen::__resized(width, height);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_algine_android_module_Bridge_renderFrame(JNIEnv *env, jclass clazz) {
    Screen::instance()->renderFrame();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_algine_android_module_Bridge_onResume(JNIEnv *env, jclass clazz) {
    event(onResume())
}

extern "C"
JNIEXPORT void JNICALL
Java_com_algine_android_module_Bridge_onPause(JNIEnv *env, jclass clazz) {
    event(onPause())
}

extern "C"
JNIEXPORT void JNICALL
Java_com_algine_android_module_Bridge_onDestroy(JNIEnv *env, jclass clazz) {
    event(onDestroy())

    Screen::destroyInstance();
    Engine::destroy();

    env->DeleteGlobalRef(asset_manager);
    env->DeleteGlobalRef(bridge);

    application.close(); // call after destroyInstance
}

// output calls

namespace AndroidBridge {
template<typename ...Args>
inline void callVoidMethod(const char *name, const char *sig, Args&& ...args) {
    executeCallback(callback_capture {
        jmethodID methodId = env->GetStaticMethodID(bridge, name, sig);
        env->CallStaticVoidMethod(bridge, methodId, args...);
    });
}

template<typename ...Args>
inline bool callBooleanMethod(const char *name, const char *sig, Args&& ...args) {
    bool result;

    executeCallback(callback_capture {
        jmethodID methodId = env->GetStaticMethodID(bridge, name, sig);
        result = env->CallStaticBooleanMethod(bridge, methodId, args...);
    });

    return result;
}

template<typename ...Args>
inline float callFloatMethod(const char *name, const char *sig, Args&& ...args) {
    float result;

    executeCallback(callback_capture {
        jmethodID methodId = env->GetStaticMethodID(bridge, name, sig);
        result = env->CallStaticFloatMethod(bridge, methodId, args...);
    });

    return result;
}

#define SIG(types) "(" types ")"
#define SIG_V(types) SIG(types) "V"
#define SIG_B(types) SIG(types) "Z"
#define SIG_F(types) SIG(types) "F"

#define SIG_ARG_no_args ""
#define SIG_ARG_int "I"

#define callVoidMethodNoArgs(name) callVoidMethod(name, SIG_V(SIG_ARG_no_args))
#define callBooleanMethodNoArgs(name) callBooleanMethod(name, SIG_B(SIG_ARG_no_args))
#define callFloatMethodNoArgs(name) callFloatMethod(name, SIG_F(SIG_ARG_no_args))

void renderLoop() {
    callVoidMethodNoArgs("renderLoop");
}

void stopRenderLoop() {
    callVoidMethodNoArgs("stopRenderLoop");
}

bool isRenderLoopRunning() {
    return callBooleanMethodNoArgs("isRenderLoopRunning");
}

bool isActionHandlerAvailable() {
    return callBooleanMethodNoArgs("isActionHandlerAvailable");
}

bool isPointerAvailable(int pointerId) {
    return callBooleanMethod("isActionHandlerAvailable", SIG_B(SIG_ARG_int), pointerId);
}

float getPointerX(int pointerId) {
    return callFloatMethod("getPointerX", SIG_F(SIG_ARG_int), pointerId);
}

float getPointerY(int pointerId) {
    return callFloatMethod("getPointerY", SIG_F(SIG_ARG_int), pointerId);
}

glm::vec2 getPointerPos(int pointerId) {
    glm::vec2 result;

    executeCallback(callback_capture {
        jmethodID methodId = env->GetStaticMethodID(bridge, "getPointerPos", "(I)[F");
        auto javaArray = (jfloatArray) env->CallStaticObjectMethod(bridge, methodId, pointerId);

        auto array = env->GetFloatArrayElements(javaArray, nullptr);

        result.x = array[0];
        result.y = array[1];

        env->ReleaseFloatArrayElements(javaArray, array, JNI_ABORT);
    });

    return result;
}

glm::ivec2 getViewDimensions() {
    glm::ivec2 result;

    executeCallback(callback_capture {
        jmethodID methodId = env->GetStaticMethodID(bridge, "getViewDimensions", "()[I");
        auto javaArray = (jintArray) env->CallStaticObjectMethod(bridge, methodId);

        auto array = env->GetIntArrayElements(javaArray, nullptr);

        result.x = array[0];
        result.y = array[1];

        env->ReleaseIntArrayElements(javaArray, array, JNI_ABORT);
    });

    return result;
}

std::string getAppDataDirectory() {
    std::string result;

    executeCallback(callback_capture {
        jclass algine_class = env->FindClass("com/algine/android/module/Algine");

        jmethodID algine_getActivity =
                env->GetStaticMethodID(algine_class, "getActivity", "()Lcom/algine/android/module/AlgineActivity;");
        jobject activity = env->CallStaticObjectMethod(algine_class, algine_getActivity);
        jclass activity_class = env->GetObjectClass(activity);

        jmethodID activity_getDataDir = env->GetMethodID(activity_class, "getDataDir", "()Ljava/io/File;");
        jobject file = env->CallObjectMethod(activity, activity_getDataDir);
        jclass file_class = env->GetObjectClass(file);

        jmethodID file_toString = env->GetMethodID(file_class, "toString", "()Ljava/lang/String;");
        auto obj_str = static_cast<jstring>(env->CallObjectMethod(file, file_toString));
        auto str = env->GetStringUTFChars(obj_str, nullptr);

        result = str;

        env->ReleaseStringUTFChars(obj_str, str);
    });

    return result;
}

void showToast(std::string_view text, int length) {
    executeCallback(callback_capture {
        jstring jstr = env->NewStringUTF(text.data());
        jmethodID methodId = env->GetStaticMethodID(bridge, "showToast", "(Ljava/lang/String;I)V");
        env->CallStaticVoidMethod(bridge, methodId, jstr, length);
    });
}
}
