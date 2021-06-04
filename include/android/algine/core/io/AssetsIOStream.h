#ifndef ALGINE_ASSETSIOSTREAM_H
#define ALGINE_ASSETSIOSTREAM_H

#include <algine/core/io/IOStream.h>

#include <tulz/assets/Asset.h>

namespace algine {
class AssetsIOStream: public IOStream {
public:
    void open(const std::string &path, IOStream::Mode mode) override;
    void close() override;

    int flush() override;

    size_t size() const override;
    size_t read(void *buffer, size_t size, size_t count) override;
    size_t seek(size_t offset, int whence) override;
    size_t tell() const override;
    size_t write(const void *buffer, size_t size, size_t count) override;

    void writeStr(const std::string &str) override;
    std::string readStr() override;

private:
    tulz::Asset m_asset;
};
}

#endif //ALGINE_ASSETSIOSTREAM_H
