#ifndef ALGINE_SHADERDEFINITIONGENERATOR_H
#define ALGINE_SHADERDEFINITIONGENERATOR_H

#include "algine/core/lua/Scriptable.h"
#include <algine/core/Pair.h>
#include <algine/types.h>

#include <string>
#include <vector>

namespace algine {
class ShaderDefinitionGenerator: public virtual Scriptable {
public:
    typedef Pair<std::string, std::string> Definition;

public:
    void define(const std::string &macro, const std::string &value = std::string());
    void define(const std::string &macro, size value);
    void removeDefinition(const std::string &macro);
    void resetDefinitions();

    void setDefinitions(const std::vector<Definition> &definitions);
    void appendDefinitions(const std::vector<Definition> &definitions);

    const std::vector<Definition>& getDefinitions() const;

    static void registerLuaUsertype(Lua *lua, sol::global_table *tenv = nullptr);

protected:
    void exec(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) override;

protected:
    std::vector<Definition> m_definitions;
};
}

#endif //ALGINE_SHADERDEFINITIONGENERATOR_H
