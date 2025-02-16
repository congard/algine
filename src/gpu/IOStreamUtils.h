#ifndef ALGINE_IOSTREAMUTILS_H
#define ALGINE_IOSTREAMUTILS_H

#include <algine/core/Engine.h>

#include <tulz/container/Array.h>

namespace algine::IOStreamUtils {
template<typename T>
inline tulz::Array<T> readAll(const std::string &path, const std::shared_ptr<IOSystem> &ioSystem) {
    auto stream = [&]() {
        if (ioSystem == nullptr) {
            return Engine::getDefaultIOSystem();
        } else {
            return ioSystem;
        }
    }()->open(path, IOStream::Mode::Read);

    auto streamSize = stream->size();
    tulz::Array<T> bytes(streamSize);

    stream->read(bytes.m_array, streamSize, 1);
    stream->close();

    return bytes;
}
}

#endif //ALGINE_IOSTREAMUTILS_H
