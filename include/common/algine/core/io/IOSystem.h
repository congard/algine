#ifndef ALGINE_IOSYSTEM_H
#define ALGINE_IOSYSTEM_H

#include <algine/core/io/IOStream.h>

#include <memory>
#include <string>

namespace algine {
class IOSystem {
public:
    virtual ~IOSystem();

    virtual std::unique_ptr<IOStream> open(const std::string &path, IOStream::Mode mode) = 0;
    virtual void close(const std::unique_ptr<IOStream> &stream) = 0;

    virtual bool exists(const std::string &path) const = 0;
    virtual bool isFile(const std::string &path) const = 0;
    virtual bool isDirectory(const std::string &path) const = 0;

    virtual char getSeparator() const = 0;
};
}

#endif //ALGINE_IOSYSTEM_H
