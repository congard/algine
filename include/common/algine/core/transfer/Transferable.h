#ifndef ALGINE_TRANSFERABLE_H
#define ALGINE_TRANSFERABLE_H

namespace algine {
class JsonHelper;

class Transferable {
public:
    virtual void import(const JsonHelper &jsonHelper) = 0;
    virtual JsonHelper dump() = 0;
};
}

#endif //ALGINE_TRANSFERABLE_H
