#ifndef ALGINE_OUTPUTLISTCREATOR_H
#define ALGINE_OUTPUTLISTCREATOR_H

#include <algine/core/transfer/Transferable.h>
#include <algine/core/OutputList.h>

#include <map>

namespace algine {
class OutputListCreator: public Transferable {
public:
    using Data = std::map<Index, Attachment>;

public:
    // do not mark explicit because we really need implicit conversion
    // just ignore Clang-Tidy warning
    OutputListCreator(const Data &data);
    OutputListCreator();

    void set(const Data &data);

    void add(Index index, Attachment attachment);
    void addColor(Index index, Index attachmentIndex);
    void addColor(Index index);

    OutputList create() const;
    const Data& data() const;

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

private:
    Data m_data;
};
}

#endif //ALGINE_OUTPUTLISTCREATOR_H
