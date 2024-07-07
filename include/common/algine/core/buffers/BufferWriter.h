#ifndef ALGINE_BUFFERWRITER_H
#define ALGINE_BUFFERWRITER_H

#include <algine/core/buffers/Buffer.h>

#include <tulz/container/Array.h>
#include <vector>

namespace algine {
class AL_EXPORT BufferWriter {
public:
    BufferWriter();
    explicit BufferWriter(Buffer *buffer);

    template<typename T>
    inline void write(uint offset, const std::vector<T> &data) {
        write(offset, data.size() * sizeof(T), data.data());
    }

    template<typename T>
    inline void write(uint offset, const tulz::Array<T> &data) {
        write(offset, data.size() * sizeof(T), data.array());
    }

    template<typename T>
    inline typename std::enable_if<std::is_fundamental<T>::value>::type write(uint offset, T data)
    {
        write(offset, sizeof(T), &data);
    }

    template<typename T>
    inline typename std::enable_if<!std::is_fundamental<T>::value>::type write(uint offset, const T &data)
    {
        write(offset, sizeof(T), &data);
    }

    void begin();
    void end();
    void write(uint offset, uint size, const void *data);

    void setBuffer(Buffer *buffer);

    Buffer* getBuffer() const;

private:
    Buffer *m_buffer = nullptr;
};
}

#endif //ALGINE_BUFFERWRITER_H
