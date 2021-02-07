#include <algine/core/log/Log.h>

namespace algine {
Logger Log::m_info {Logger::Type::Info};
Logger Log::m_error {Logger::Type::Error};

// Android inserts prefixes (I/, E/ etc) automatically

Logger& Log::info(const std::string &tag) {
    enable_if_desktop(m_info.setTag("I/" + tag));
    enable_if_android(m_info.setTag(tag));

    return m_info;
}

Logger& Log::error(const std::string &tag) {
    enable_if_desktop(m_error.setTag("E/" + tag));
    enable_if_android(m_error.setTag(tag));

    return m_error;
}
}
