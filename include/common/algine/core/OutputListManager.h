#ifndef ALGINE_OUTPUTLISTMANAGER_H
#define ALGINE_OUTPUTLISTMANAGER_H

#include <algine/core/transfer/Transferable.h>
#include <algine/core/OutputList.h>

#include <map>

namespace algine {
class OutputListManager: public Transferable {
public:
    using Data = std::map<Index, Attachment>;

public:
    // do not mark explicit because we really need implicit conversion
    // just ignore Clang-Tidy warning
    OutputListManager(const Data &data);
    OutputListManager();

    void set(const Data &data);

    void add(Index index, Attachment attachment);
    void addColor(Index index, Index attachmentIndex);
    void addColor(Index index);

    OutputList get() const;
    const Data& data() const;

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

private:
    Data m_data;
};
}

#endif //ALGINE_OUTPUTLISTMANAGER_H
