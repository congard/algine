#ifndef ALGINE_DJB2_H
#define ALGINE_DJB2_H

namespace algine {
// Source: http://www.cse.yorku.ca/~oz/hash.html
inline unsigned long hash_djb2(unsigned char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}
}

#endif //ALGINE_DJB2_H
