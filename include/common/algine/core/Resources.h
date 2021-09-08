#ifndef ALGINE_RESOURCES_H
#define ALGINE_RESOURCES_H

#include <algine/core/widgets/Dimen.h>
#include <algine/core/Variant.h>
#include <algine/core/Engine.h>

#include <variant>
#include <memory>
#include <map>

namespace pugi {
class xml_node;
class xml_document;
}

namespace algine {
class Resources {
public:
    using Resource = Variant<std::string, int, float, Dimen>;

public:
    void setStrings(const std::map<std::string, std::string> &strings);
    void setInts(const std::map<std::string, int> &ints);
    void setFloats(const std::map<std::string, float> &floats);
    void setDimens(const std::map<std::string, Dimen> &dimens);

    const std::map<std::string, std::string>& getStrings() const;
    const std::map<std::string, int>& getInts() const;
    const std::map<std::string, float>& getFloats() const;
    const std::map<std::string, Dimen>& getDimens() const;

    void setString(std::string_view name, std::string_view value);
    void setInt(std::string_view name, int value);
    void setFloat(std::string_view name, float value);
    void setDimen(std::string_view name, Dimen value);

    bool hasString(const std::string &name) const;
    bool hasInt(const std::string &name) const;
    bool hasFloat(const std::string &name) const;
    bool hasDimen(const std::string &name) const;

    const std::string& getString(const std::string &name) const;
    int getInt(const std::string &name) const;
    float getFloat(const std::string &name) const;
    const Dimen& getDimen(const std::string &name) const;

    Resource parse(std::string_view str, bool *error = nullptr) const;

    void fromXML(const pugi::xml_node &node);
    bool fromXML(const std::string &xml);
    bool fromXMLFile(const std::string &file, const std::shared_ptr<IOSystem> &io = Engine::getDefaultIOSystem());

    pugi::xml_document toXML() const;
    std::string toXMLString() const;

    static Resources* instance();

private:
    static std::unique_ptr<Resources> m_instance;

private:
    std::map<std::string, std::string> m_strings;
    std::map<std::string, int> m_ints;
    std::map<std::string, float> m_floats;
    std::map<std::string, Dimen> m_dimens;
};
}

#endif //ALGINE_RESOURCES_H
