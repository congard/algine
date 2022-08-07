#ifndef ALGINE_CREATOR_H
#define ALGINE_CREATOR_H

#include <algine/core/lua/Scriptable.h>

#include <string>

namespace algine {
class Creator: public virtual Scriptable {
public:
    enum class Access {
        Private,
        Public
    };

public:
    Creator();

    void setName(const std::string &name);
    void setAccess(Access access);

    const std::string& getName() const;
    Access getAccess() const;

protected:
    std::string m_name;
    Access m_access;
};
}

#endif //ALGINE_CREATOR_H
