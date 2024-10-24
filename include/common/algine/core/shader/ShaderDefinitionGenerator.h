#ifndef ALGINE_SHADERDEFINITIONGENERATOR_H
#define ALGINE_SHADERDEFINITIONGENERATOR_H

#include <algine/core/lua/Scriptable.h>
#include <algine/types.h>

#include <string>
#include <vector>

namespace algine {
// TODO: naming, it is not a generator
class AL_EXPORT ShaderDefinitionGenerator: public virtual Scriptable {
public:
    // TODO: struct instead
    using Definition = std::pair<std::string, std::string>;

public:
    void define(const std::string &macro, const std::string &value = std::string());
    void define(const std::string &macro, size value);
    void removeDefinition(const std::string &macro);
    void resetDefinitions();

    void setDefinitions(const std::vector<Definition> &definitions);
    void appendDefinitions(const std::vector<Definition> &definitions);

    const std::vector<Definition>& getDefinitions() const;

protected:
    void exec(const std::string &s, bool path, Lua *lua, sol::environment *tenv) override;

protected:
    std::vector<Definition> m_definitions;
};
}

#endif //ALGINE_SHADERDEFINITIONGENERATOR_H
