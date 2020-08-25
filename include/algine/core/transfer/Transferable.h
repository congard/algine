#ifndef ALGINE_TRANSFERABLE_H
#define ALGINE_TRANSFERABLE_H

#include <string>
#include <glm/matrix.hpp>

namespace algine {
class JsonHelper;

class Transferable {
public:
    virtual void import(const JsonHelper &jsonHelper) = 0;
    virtual JsonHelper dump() = 0;

    virtual void importFromFile(const std::string &path);
};
}

#endif //ALGINE_TRANSFERABLE_H
