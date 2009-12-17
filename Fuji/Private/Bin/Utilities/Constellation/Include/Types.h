#if !defined(_TYPES_H)
#define _TYPES_H

typedef unsigned char uint8;
typedef signed char int8;
typedef unsigned short int uint16;
typedef signed short int int16;
typedef unsigned long int uint32;
typedef signed long int int32;

#if defined(_MSC_VER)
typedef unsigned __int64 uint64;
typedef __int64 int64;
#else
typedef unsigned long long uint64;
typedef long long int64;
#endif

#endif