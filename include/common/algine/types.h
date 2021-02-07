#ifndef ALGINE_TYPES_H
#define ALGINE_TYPES_H

namespace algine {
typedef char int8;
typedef unsigned char uint8;
typedef int8 byte;
typedef uint8 ubyte;
typedef uint8 uchar;
typedef short int16;
typedef unsigned short uint16;
typedef unsigned int uint;
typedef long int64;
typedef unsigned long uint64;
typedef double float64;

#if INTPTR_MAX == INT64_MAX
typedef unsigned long usize;
typedef long size;
#elif INTPTR_MAX == INT32_MAX
typedef unsigned int usize;
typedef int size;
#else
#error "Error in types.h: are you sure that you have a 32 or 64 bit system? If compatibility with 16-bit and 8-bit systems is needed, write here: https://gitlab.com/congard/algine/issues or dbcongard@gmail.com"
#endif

typedef uint Index;
typedef uint Attachment;
} /* namespace algine */

#endif /* ALGINE_TYPES_H */