#include <algine/core/io/AssetsIOStream.h>

#include <stdexcept>

using namespace tulz;

namespace algine {
inline void readOnlyError() {
    throw std::runtime_error("You cannot use AssetsIOStream for write operations. It is read-only.");
}

void AssetsIOStream::open(const std::string &path, IOStream::Mode mode) {
    if (mode != IOStream::Mode::Read && mode != IOStream::Mode::ReadText) {
        readOnlyError();
    }

    m_asset.open(path, Asset::Mode::Unknown);
}

void AssetsIOStream::close() {
    m_asset.close();
}

int AssetsIOStream::flush() {
    readOnlyError();
    return 0;
}

size_t AssetsIOStream::size() const {
    return m_asset.getLength();
}

size_t AssetsIOStream::read(void *buffer, size_t size, size_t count) {
    return m_asset.read(buffer, size * count);
}

size_t AssetsIOStream::seek(size_t offset, int whence) {
    return m_asset.seek(offset, whence);
}

size_t AssetsIOStream::tell() const {
    return m_asset.tell();
}

size_t AssetsIOStream::write(const void *buffer, size_t size, size_t count) {
    readOnlyError();
    return 0;
}

void AssetsIOStream::writeStr(const std::string &str) {
    readOnlyError();
}

std::string AssetsIOStream::readStr() {
    return m_asset.getString();
}
}
