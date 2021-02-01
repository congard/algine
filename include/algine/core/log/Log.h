#ifndef ALGINE_LOG_H
#define ALGINE_LOG_H

#include <algine/core/log/Logger.h>

namespace algine {
class Log {
public:
    static Logger& info(const std::string &tag = "Algine");
    static Logger& error(const std::string &tag = "Algine");

private:
    static Logger m_info;
    static Logger m_error;
};
}


#endif //ALGINE_LOG_H
