#ifndef ALGINE_OUTPUTLIST_H
#define ALGINE_OUTPUTLIST_H

#include <algine/core/lua/Scriptable.h>
#include <algine/types.h>

#include <vector>
#include <map>
#include <cstddef>

namespace algine {
class OutputList: public Scriptable {
public:
    // do not mark explicit because we really need implicit conversion
    // just ignore Clang-Tidy warning
    OutputList(const std::map<Index, Attachment> &list);
    OutputList();

    void set(const std::map<Index, Attachment> &list);
    void set(const std::vector<Attachment> &list);
    void add(Index index, Attachment attachment);
    void addColor(Index index, Index attachmentIndex);
    void addColor(Index index);
    void remove(Index index, bool optimizeList = true);
    void removeAll();
    void optimize();

    size_t size() const;
    const std::vector<Attachment>& get() const;
    const Attachment* data() const;

protected:
    void exec(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) override;

private:
    std::vector<Attachment> m_list;
};
}

#endif //ALGINE_OUTPUTLIST_H
