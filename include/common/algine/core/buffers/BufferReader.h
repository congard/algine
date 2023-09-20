#ifndef ALGINE_BUFFERREADER_H
#define ALGINE_BUFFERREADER_H

#include <algine/core/buffers/Buffer.h>

#include <tulz/Array.h>

namespace algine {
class AL_EXPORT BufferReader {
public:
    BufferReader();
    explicit BufferReader(Buffer *buffer);

    void begin();
    void end();

    tulz::Array<byte> read(uint offset, uint size);

    template<typename T>
    inline T read(uint offset) {
        // return copy of the object from bytes
        return *reinterpret_cast<T*>(read(offset, sizeof(T)).array());
    }

    void setBuffer(Buffer *buffer);

    Buffer* getBuffer() const;

private:
    Buffer *m_buffer = nullptr;
};
}

#endif //ALGINE_BUFFERREADER_H
