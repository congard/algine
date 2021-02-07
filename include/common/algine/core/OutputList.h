#ifndef ALGINE_OUTPUTLIST_H
#define ALGINE_OUTPUTLIST_H

#include <algine/types.h>

#include <vector>

namespace algine {
class OutputList {
public:
    OutputList();

    void set(const std::vector<Attachment> &list);
    void add(Index index, Attachment attachment);
    void addColor(Index index, Index attachmentIndex);
    void addColor(Index index);
    void remove(Index index, bool optimizeList = true);
    void optimize();

    size_t size() const;
    const std::vector<Attachment>& get() const;
    const Attachment* data() const;

private:
    std::vector<Attachment> m_list;
};
}

#endif //ALGINE_OUTPUTLIST_H
