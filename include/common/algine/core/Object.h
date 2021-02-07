#ifndef ALGINE_OBJECT_H
#define ALGINE_OBJECT_H

#include <algine/core/transfer/Transferable.h>

#include <string>

namespace algine {
class Object: public Transferable {
public:
    void setName(const std::string &name);

    std::string getName() const;

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

public:
    std::string name;
};
}

#endif //ALGINE_OBJECT_H
