#ifndef ALGINE_ALIGNMENT_H
#define ALGINE_ALIGNMENT_H

#include <algine/types.h>

namespace algine {
class Alignment {
public:
    enum {
        Center  = 0b0000,
        Left    = 0b0001,
        Right   = 0b0010,
        Top     = 0b0100,
        Bottom  = 0b1000
    };

public:
    static uint parse(const char *str);
};
}

#endif //ALGINE_ALIGNMENT_H
