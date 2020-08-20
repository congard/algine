#ifndef ALGINE_SERIALIZABLE_H
#define ALGINE_SERIALIZABLE_H

#include <string>

namespace algine {
class JsonHelper;

class Serializable {
public:
    virtual void deserialize(const JsonHelper &jsonHelper) = 0;
    virtual JsonHelper serialize() = 0;

    void deserializeFromFile(const std::string &path);
};
}

#endif //ALGINE_SERIALIZABLE_H
