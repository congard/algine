#include <algine/core/OutputList.h>
#include <algine/core/Framebuffer.h>

#include <stdexcept>

using namespace std;

namespace algine {
OutputList::OutputList(const std::map<Index, Attachment> &list) {
    set(list);
}

OutputList::OutputList()
    : m_list({Framebuffer::ColorAttachmentZero}) {}

void OutputList::set(const std::map<Index, Attachment> &list) {
    for (const auto &p : list) {
        add(p.first, p.second);
    }
}

void OutputList::set(const vector<Attachment> &list) {
    m_list = list;
}

void OutputList::add(Index index, Attachment attachment) {
    if (index == m_list.size()) {
        m_list.emplace_back(attachment);
    } else if (index > m_list.size()) {
        m_list.resize(index + 1, Framebuffer::EmptyAttachment);
        m_list[index] = attachment;
    } else {
        m_list[index] = attachment;
    }
}

void OutputList::addColor(Index index, Index attachmentIndex) {
    add(index, Framebuffer::ColorAttachmentZero + attachmentIndex);
}

void OutputList::addColor(Index index) {
    add(index, Framebuffer::ColorAttachmentZero + index);
}

void OutputList::remove(Index index, bool optimizeList) {
    if (index < m_list.size()) {
        m_list[index] = Framebuffer::EmptyAttachment;
    } else {
        throw runtime_error(
            "Incorrect index value."
            "\nList size: " + to_string(m_list.size()) +
            "\nGiven index: " + to_string(index)
        );
    }

    if (optimizeList && index == m_list.size() - 1) {
        optimize();
    }
}

void OutputList::removeAll() {
    m_list.clear();
}

void OutputList::optimize() {
    for (int i = static_cast<int>(m_list.size()) - 1; i >= 0; --i) {
        if (m_list[i] != Framebuffer::EmptyAttachment) {
            m_list.erase(m_list.begin() + i + 1, m_list.end());
            return;
        }
    }

    m_list.clear();
}

size_t OutputList::size() const {
    return m_list.size();
}

const vector<Attachment>& OutputList::get() const {
    return m_list;
}

const Attachment* OutputList::data() const {
    return m_list.data();
}

void OutputList::registerLuaUsertype(Lua *lua, sol::global_table *tenv) {
    auto &env = getEnv(lua, tenv);

    if (isRegistered(env, "OutputList"))
        return;

    lua->registerUsertype<Scriptable>(tenv);

    auto ctors = sol::constructors<OutputList()>();
    auto usertype = env.new_usertype<OutputList>(
            "OutputList",
            sol::meta_function::length, &OutputList::size,
            sol::meta_function::construct, ctors,
            sol::call_constructor, ctors,
            sol::base_classes, sol::bases<Scriptable>());

    usertype["set"] = sol::overload(
        [](OutputList &self, std::vector<Attachment> list) { self.set(list); },
        [](OutputList &self, const sol::table &list) {
            self.removeAll();
            for (const auto &p : list) self.add(p.first.as<Index>(), p.second.as<Attachment>());
        }
    );
    usertype["add"] = &OutputList::add;
    usertype["addColor"] = sol::overload(
        static_cast<void (OutputList::*)(Index, Index)>(&OutputList::addColor),
        static_cast<void (OutputList::*)(Index)>(&OutputList::addColor)
    );
    usertype["remove"] = sol::overload(
        &OutputList::remove,
        [](OutputList &self, Index index) { self.remove(index); }
    );
    usertype["removeAll"] = &OutputList::removeAll;
    usertype["optimize"] = &OutputList::optimize;
    usertype["size"] = &OutputList::size;
    usertype["get"] = &OutputList::get;
    usertype["data"] = &OutputList::data;
}

void OutputList::exec(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) {
    exec_t<OutputList>(s, path, lua, tenv);
}
}
