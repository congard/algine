#include <algine/core/Resources.h>
#include <algine/core/log/Log.h>
#include <algine/core/widgets/Units.h>

#include <pugixml.hpp>

#include <cstring>

#define TAG "Resources"
#define Key_String "string"
#define Key_Int "int"
#define Key_Float "float"
#define Key_Dimen "dimen"

namespace algine {
std::unique_ptr<Resources> Resources::m_instance {nullptr};

void Resources::setStrings(const std::map<std::string, std::string> &strings) {
    m_strings = strings;
}

void Resources::setInts(const std::map<std::string, int> &ints) {
    m_ints = ints;
}

void Resources::setFloats(const std::map<std::string, float> &floats) {
    m_floats = floats;
}

void Resources::setDimens(const std::map<std::string, Dimen> &dimens) {
    m_dimens = dimens;
}

const std::map<std::string, std::string>& Resources::getStrings() const {
    return m_strings;
}

const std::map<std::string, int>& Resources::getInts() const {
    return m_ints;
}

const std::map<std::string, float>& Resources::getFloats() const {
    return m_floats;
}

const std::map<std::string, Dimen>& Resources::getDimens() const {
    return m_dimens;
}

void Resources::setString(std::string_view name, std::string_view value) {
    m_strings[std::string(name)] = value;
}

void Resources::setInt(std::string_view name, int value) {
    m_ints[std::string(name)] = value;
}

void Resources::setFloat(std::string_view name, float value) {
    m_floats[std::string(name)] = value;
}

void Resources::setDimen(std::string_view name, Dimen value) {
    m_dimens[std::string(name)] = value;
}

template<typename T>
inline auto has(const T &m, const std::string &name) {
    return m.find(name) != m.end();
}

bool Resources::hasString(const std::string &name) const {
    return has(m_strings, name);
}

bool Resources::hasInt(const std::string &name) const {
    return has(m_ints, name);
}

bool Resources::hasFloat(const std::string &name) const {
    return has(m_floats, name);
}

bool Resources::hasDimen(const std::string &name) const {
    return has(m_dimens, name);
}

const std::string& Resources::getString(const std::string &name) const {
    return m_strings.at(name);
}

int Resources::getInt(const std::string &name) const {
    return m_ints.at(name);
}

float Resources::getFloat(const std::string &name) const {
    return m_floats.at(name);
}

const Dimen& Resources::getDimen(const std::string &name) const {
    return m_dimens.at(name);
}

Resources::Resource Resources::parse(std::string_view str_v, bool *error) const {
    // @string/name
    // @int/name
    // @float/name
    // @dimen/name

    auto setError = [&](bool e) { if (error) *error = e; };

    auto str = str_v.data();

    if (str[0] != '@') {
        setError(true);
        return std::string(str_v);
    }

    ++str;

    enum Type {
        Unknown, String, Int, Float, Dimen
    };

    Type type {Unknown};

    auto str_len = strlen(str);

    auto isType = [&](const char *key, Type _type) {
        auto key_len = strlen(key);

        if (strncmp(str, key, key_len) == 0) {
            str_len -= key_len + 1; // +1 because of /
            str += key_len + 1;
            type = _type;
            return true;
        }

        return false;
    };

    isType(Key_String, String) || isType(Key_Int, Int) || isType(Key_Float, Float) || isType(Key_Dimen, Dimen);

    auto getExistingFrom = [&](const auto &m) -> typename std::remove_reference_t<decltype(m)>::mapped_type {
        if (auto it = m.find(str); it != m.end()) {
            setError(false);
            return (*it).second;
        } else {
            setError(true);
            Log::error(TAG) << "Resource " << str_v.data() << " does not exist" << Log::end;
            return {};
        }
    };

    switch (type) {
        case String: return getExistingFrom(m_strings);
        case Int: return getExistingFrom(m_ints);
        case Float: return getExistingFrom(m_floats);
        case Dimen: return getExistingFrom(m_dimens);
        default: {
            setError(true);
            Log::error(TAG) << "Unknown resource type: " << str_v.data() << Log::end;
            return std::string(str_v);
        }
    }
}

#define XML_Root_Node "resources"

void Resources::fromXML(const pugi::xml_node &node) {
    if (strcmp(node.name(), XML_Root_Node) != 0) {
        throw std::runtime_error("Resources: Broken XML: it must starts with " XML_Root_Node " tag");
    }

    for (pugi::xml_node child : node.children()) {
        pugi::xml_attribute attrib = child.attribute("name");

        auto isChild = [&](const char *name) {
            return strcmp(child.name(), name) == 0;
        };

        auto name = attrib.value();
        auto value = child.text();

        if (isChild(Key_String)) {
            setString(name, value.as_string());
        } else if (isChild(Key_Int)) {
            setInt(name, value.as_int());
        } else if (isChild(Key_Float)) {
            setFloat(name, value.as_float());
        } else if (isChild(Key_Dimen)) {
            setDimen(name, Units::parse_dimen(value.as_string()));
        } else {
            Log::error(TAG) << "Unknown resource type: " << child.name() << Log::end;
        }
    }
}

bool Resources::fromXML(const std::string &xml) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(xml.c_str());

    if (!result) {
        Log::error("Resources XML Parser") << result.description() << Log::end;
        return false;
    } else {
        fromXML(doc.document_element());
        return true;
    }
}

bool Resources::fromXMLFile(const std::string &file, const std::shared_ptr<IOSystem> &io) {
    auto stream = io->open(file, IOStream::Mode::ReadText);
    auto xml = stream->readStr();
    stream->close();
    return fromXML(xml);
}

pugi::xml_document Resources::toXML() const {
    pugi::xml_document doc;

    pugi::xml_node root = doc.append_child(XML_Root_Node);

    auto appendNodes = [&](const auto &map, const char *type, auto converter) {
        for (auto &p : map) {
            pugi::xml_node node = root.append_child(type);
            node.append_attribute("name") = p.first.c_str();
            node.append_child(pugi::node_pcdata).set_value(converter(p.second).c_str());
        }
    };

    appendNodes(m_strings, Key_String, [](const std::string &str) -> auto& { return str; });
    appendNodes(m_ints, Key_Int, [](int i) { return std::to_string(i); });
    appendNodes(m_floats, Key_Float, [](float f) { return std::to_string(f); });
    appendNodes(m_dimens, Key_Dimen, [](const Dimen &dimen) { return dimen.toString(); });

    return doc;
}

std::string Resources::toXMLString() const {
    struct : pugi::xml_writer {
        std::string result;

        void write(const void* data, size_t size) override {
            result.append(static_cast<const char*>(data), size);
        }
    } writer;

    auto doc = toXML();
    doc.print(writer);

    return writer.result;
}

Resources* Resources::instance() {
    if (!m_instance)
        m_instance = std::make_unique<Resources>();

    return m_instance.get();
}
}
