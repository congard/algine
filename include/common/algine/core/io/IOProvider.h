#ifndef ALGINE_IOPROVIDER_H
#define ALGINE_IOPROVIDER_H

#include <algine/core/io/IOSystem.h>
#include <algine/core/lua/Lua.h>

#include <memory>

namespace algine {
class IOProvider {
public:
    IOProvider();
    virtual ~IOProvider() = default;

    void setIOSystem(IOSystem *ioSystem);
    void setIOSystem(const std::shared_ptr<IOSystem> &ioSystem);
    const std::shared_ptr<IOSystem>& getIOSystem() const;

    void writeStr(const std::string &path, const std::string &str) const;
    std::string readStr(const std::string &path) const;

    static void registerLuaUsertype(Lua *lua, sol::global_table *tenv);

protected:
    const std::shared_ptr<IOSystem>& io() const;

private:
    std::shared_ptr<IOSystem> m_ioSystem;
};
}

#endif //ALGINE_IOPROVIDER_H
