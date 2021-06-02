#ifndef ALGINE_IOSTREAM_H
#define ALGINE_IOSTREAM_H

#include <string>

namespace algine {
class IOStream {
public:
    enum class Mode {
        None,
        ReadText,
        Read,
        WriteText,
        Write,
        AppendText,
        Append
    };

public:
    IOStream();
    virtual ~IOStream();

    virtual void open(const std::string &path, IOStream::Mode mode) = 0;
    virtual void close() = 0;

    virtual int flush() = 0;

    virtual size_t size() const = 0;
    virtual size_t read(void *buffer, size_t size, size_t count) = 0;
    virtual size_t seek(size_t offset, int whence) = 0;
    virtual size_t tell() const = 0;
    virtual size_t write(const void *buffer, size_t size, size_t count) = 0;

    virtual void writeStr(const std::string &str) = 0;
    virtual std::string readStr() = 0;
};
}

#endif //ALGINE_IOSTREAM_H
