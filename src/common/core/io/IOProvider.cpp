#include <algine/core/io/IOProvider.h>
#include <algine/core/Engine.h>

#include <utility>

namespace algine {
IOProvider::IOProvider()
    : m_ioSystem(nullptr) {}

IOProvider::IOProvider(std::shared_ptr<IOSystem> ioSystem)
    : m_ioSystem(std::move(ioSystem)) {}

void IOProvider::setIOSystem(IOSystem *ioSystem) {
    m_ioSystem.reset(ioSystem);
}

void IOProvider::setIOSystem(const std::shared_ptr<IOSystem> &ioSystem) {
    m_ioSystem = ioSystem;
}

const std::shared_ptr<IOSystem>& IOProvider::getIOSystem() const {
    return io();
}

void IOProvider::writeStr(const std::string &path, const std::string &str) const {
    auto stream = io()->open(path, IOStream::Mode::WriteText);
    stream->writeStr(str);
    stream->close();
}

std::string IOProvider::readStr(const std::string &path) const {
    auto stream = io()->open(path, IOStream::Mode::ReadText);
    std::string str = stream->readStr();
    stream->close();
    return str;
}

bool IOProvider::exists(const std::string &path) const {
    return io()->exists(path);
}

const std::shared_ptr<IOSystem>& IOProvider::io() const {
    if (m_ioSystem == nullptr) {
        return Engine::getDefaultIOSystem();
    } else {
        return m_ioSystem;
    }
}
}
