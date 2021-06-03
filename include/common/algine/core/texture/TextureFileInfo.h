#ifndef ALGINE_TEXTUREFILEINFO_H
#define ALGINE_TEXTUREFILEINFO_H

#include <algine/core/DataType.h>
#include <algine/core/io/IOSystem.h>

#include <memory>
#include <string>

namespace algine {
struct TextureFileInfo {
    std::string path;
    std::shared_ptr<IOSystem> ioSystem;
    DataType dataType {DataType::UnsignedByte};
    bool flip {false};
};
}

#endif //ALGINE_TEXTUREFILEINFO_H
