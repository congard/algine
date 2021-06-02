#include <algine/core/io/StandardIOSystem.h>
#include <algine/core/io/StandardIOStream.h>

#include <tulz/Path.h>

#include <memory>

using namespace tulz;

namespace algine {
std::unique_ptr<IOStream> StandardIOSystem::open(const std::string &path, IOStream::Mode mode) {
    auto ptr = std::make_unique<StandardIOStream>();
    ptr->open(path, mode);
    return ptr;
}

void StandardIOSystem::close(const std::unique_ptr<IOStream> &stream) {
    stream->close();
}

bool StandardIOSystem::exists(const std::string &path) const {
    return Path(path).exists();
}

bool StandardIOSystem::isFile(const std::string &path) const {
    return Path(path).isFile();
}

bool StandardIOSystem::isDirectory(const std::string &path) const {
    return Path(path).isDirectory();
}

char StandardIOSystem::getSeparator() const {
    return Path::SystemSeparator;
}
}
