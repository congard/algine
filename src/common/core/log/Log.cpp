#include <algine/core/log/Log.h>
#include <algine/core/Engine.h>

namespace algine {
// Android inserts prefixes (I/, E/ etc) automatically

#ifdef __ANDROID__
    #include <android/log.h>
    #define LOG(tag, priority, format, ...) __android_log_print(priority, tag.c_str(), format, __VA_ARGS__)
    #define LOG_INFO(tag, format, ...) LOG(tag, ANDROID_LOG_INFO, format, __VA_ARGS__)
    #define LOG_ERROR(tag, format, ...) LOG(tag, ANDROID_LOG_ERROR, format, __VA_ARGS__)
    #define LOG_INFO_TAG(tag) tag
    #define LOG_ERROR_TAG(tag) tag
#else
    #include <cstdio>
    #define LOG(tag, stream, format, ...) fprintf(stream, (tag + ": " format "\n").c_str(), __VA_ARGS__)
    #define LOG_INFO(tag, format, ...) LOG(tag, stdout, format, __VA_ARGS__)
    #define LOG_ERROR(tag, format, ...) LOG(tag, stderr, format, __VA_ARGS__)
    #define LOG_INFO_TAG(tag) ("I/" + (tag))
    #define LOG_ERROR_TAG(tag) ("E/" + (tag))
#endif

enum LoggerId {
    Info, Error
};

void Log::InputEndListener::onInputEnd(Logger &logger) {
    auto &tag = logger.getTag();
    auto log = logger.str();

    if (logger.getId() == Info) {
        Log::info(tag, log);
    } else {
        Log::error(tag, log);
    }
}

std::mutex Log::m_mutex;
Log::InputEndListener Log::m_endListener;

Logger Log::info(const std::string &tag) {
    Logger logger;
    logger.setId(Info);
    logger.setTag(tag);
    logger.setInputEndListener(&m_endListener);
    return logger;
}

Logger Log::error(const std::string &tag) {
    Logger logger;
    logger.setId(Error);
    logger.setTag(tag);
    logger.setInputEndListener(&m_endListener);
    return logger;
}

void Log::info(const std::string &tag, std::string_view str) {
    std::lock_guard lockGuard(m_mutex);
    LOG_INFO(LOG_INFO_TAG(tag), "%s", str.data());
}

void Log::error(const std::string &tag, std::string_view str) {
    std::lock_guard lockGuard(m_mutex);
    LOG_ERROR(LOG_ERROR_TAG(tag), "%s", str.data());
}

void Log::registerLuaUsertype(Lua *lua) {
    lua = lua ? lua : &Engine::getLua();
    auto &state = *lua->state();

    if (state["Log"].valid())
        return;

    auto usertype = state.new_usertype<Log>(
            "Log",
            sol::call_constructor, sol::no_constructor,
            sol::meta_function::construct, sol::no_constructor);

    usertype["info"] = sol::overload(
        static_cast<void (*)(const std::string&, std::string_view)>(&Log::info),
        [](std::string_view str) { Log::info("AlgineLua", str); }
    );

    usertype["error"] = sol::overload(
        static_cast<void (*)(const std::string&, std::string_view)>(&Log::error),
        [](std::string_view str) { Log::error("AlgineLua", str); }
    );
}
}
