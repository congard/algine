#ifndef ALGINE_CREATOR_H
#define ALGINE_CREATOR_H

#include <algine/core/transfer/FileTransferable.h>

#include <string>

namespace algine {
class Creator: public FileTransferable {
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

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

    static void registerLuaUsertype(Lua *lua, sol::global_table *tenv);

protected:
    std::string m_name;
    Access m_access;
};
}

#endif //ALGINE_CREATOR_H
