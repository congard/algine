#ifndef ALGINE_IOWRAPPER_H
#define ALGINE_IOWRAPPER_H

#include <algine/core/io/IOSystem.h>

#include <memory>

namespace algine {
class IOWrapper {
public:
    IOWrapper();

    void setIOSystem(IOSystem *ioSystem);
    void setIOSystem(const std::shared_ptr<IOSystem> &ioSystem);
    const std::shared_ptr<IOSystem>& getIOSystem() const;

    void writeStr(const std::string &path, const std::string &str) const;
    std::string readStr(const std::string &path) const;

protected:
    const std::shared_ptr<IOSystem>& io() const;

private:
    std::shared_ptr<IOSystem> m_ioSystem;
};
}

#endif //ALGINE_IOWRAPPER_H
