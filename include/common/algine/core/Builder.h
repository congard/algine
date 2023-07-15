#ifndef ALGINE_BUILDER_H
#define ALGINE_BUILDER_H

#include <algine/core/lua/Scriptable.h>

#include <string>

namespace algine {
class Builder: public virtual Scriptable {
public:
    // TODO: remove after Scene class creation
    enum class Access {
        Private,
        Public
    };

public:
    Builder();

    void setName(const std::string &name);
    void setAccess(Access access);

    const std::string& getName() const;
    Access getAccess() const;

protected:
    std::string m_name;
    Access m_access;
};
}

#endif //ALGINE_BUILDER_H
