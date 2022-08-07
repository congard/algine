#ifndef ALGINE_OBJECT_H
#define ALGINE_OBJECT_H

#include <algine/core/lua/Scriptable.h>

#include <string>

namespace algine {
class Object {
public:
    void setName(const std::string &name);
    const std::string& getName() const;

private:
    std::string m_name;
};
}

#endif //ALGINE_OBJECT_H
