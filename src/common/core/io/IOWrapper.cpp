#include <algine/core/io/IOWrapper.h>
#include <algine/core/Engine.h>

namespace algine {
IOWrapper::IOWrapper()
    : m_ioSystem(nullptr) {}

void IOWrapper::setIOSystem(IOSystem *ioSystem) {
    m_ioSystem.reset(ioSystem);
}

void IOWrapper::setIOSystem(const std::shared_ptr<IOSystem> &ioSystem) {
    m_ioSystem = ioSystem;
}

const std::shared_ptr<IOSystem>& IOWrapper::getIOSystem() const {
    return io();
}

void IOWrapper::writeStr(const std::string &path, const std::string &str) const {
    auto stream = io()->open(path, IOStream::Mode::WriteText);
    stream->writeStr(str);
    stream->close();
}

std::string IOWrapper::readStr(const std::string &path) const {
    auto stream = io()->open(path, IOStream::Mode::ReadText);
    std::string str = stream->readStr();
    stream->close();
    return str;
}

const std::shared_ptr<IOSystem>& IOWrapper::io() const {
    if (m_ioSystem == nullptr) {
        return Engine::getDefaultIOSystem();
    } else {
        return m_ioSystem;
    }
}
}
