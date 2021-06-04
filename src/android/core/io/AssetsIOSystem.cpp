#include <algine/core/io/AssetsIOSystem.h>
#include <algine/core/io/AssetsIOStream.h>

#include <tulz/assets/AssetManager.h>

using namespace tulz;

namespace algine {
std::unique_ptr<IOStream> AssetsIOSystem::open(const std::string &path, IOStream::Mode mode) {
    auto ptr = std::make_unique<AssetsIOStream>();
    ptr->open(path, mode);
    return ptr;
}

void AssetsIOSystem::close(const std::unique_ptr<IOStream> &stream) {
    stream->close();
}

bool AssetsIOSystem::exists(const std::string &path) const {
    return AssetManager::exists(path);
}

bool AssetsIOSystem::isFile(const std::string &path) const {
    return AssetManager::isFile(path);
}

bool AssetsIOSystem::isDirectory(const std::string &path) const {
    return AssetManager::isDirectory(path);
}

char AssetsIOSystem::getSeparator() const {
    return '/';
}
}
