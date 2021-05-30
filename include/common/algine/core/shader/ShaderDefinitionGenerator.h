#ifndef ALGINE_SHADERDEFINITIONGENERATOR_H
#define ALGINE_SHADERDEFINITIONGENERATOR_H

#include <algine/core/transfer/Transferable.h>
#include <algine/types.h>

#include <string>
#include <vector>

namespace algine {
class ShaderDefinitionGenerator: public Transferable {
public:
    typedef std::pair<std::string, std::string> Definition;

public:
    void define(const std::string &macro, const std::string &value = std::string());
    void define(const std::string &macro, size value);
    void removeDefinition(const std::string &macro);
    void resetDefinitions();

    void setDefinitions(const std::vector<Definition> &definitions);
    void appendDefinitions(const std::vector<Definition> &definitions);

    const std::vector<Definition>& getDefinitions() const;

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

protected:
    std::vector<Definition> m_definitions;
};
}

#endif //ALGINE_SHADERDEFINITIONGENERATOR_H
