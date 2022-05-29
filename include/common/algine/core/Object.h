#ifndef ALGINE_OBJECT_H
#define ALGINE_OBJECT_H

#include <algine/core/transfer/Transferable.h>
#include <algine/core/lua/Scriptable.h>

#include <string>

namespace algine {
class Object: public Transferable, public Scriptable {
public:
    void setName(const std::string &name);
    const std::string& getName() const;

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

    static void registerLuaUsertype(Lua *lua);

private:
    std::string m_name;
};
}

#endif //ALGINE_OBJECT_H
