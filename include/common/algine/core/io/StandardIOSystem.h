#ifndef ALGINE_STANDARDIOSYSTEM_H
#define ALGINE_STANDARDIOSYSTEM_H

#include <algine/core/io/IOSystem.h>

namespace algine {
class AL_EXPORT StandardIOSystem: public IOSystem {
public:
    std::unique_ptr<IOStream> open(const std::string &path, IOStream::Mode mode) override;
    void close(const std::unique_ptr<IOStream> &stream) override;

    bool exists(const std::string &path) const override;
    bool isFile(const std::string &path) const override;
    bool isDirectory(const std::string &path) const override;

    char getSeparator() const override;
};
}

#endif //ALGINE_STANDARDIOSYSTEM_H
