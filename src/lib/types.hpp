typedef signed char i8;
typedef short i16;
typedef int i32;
typedef long long i64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef unsigned int u32;

typedef float f32;
typedef double f64;

typedef char utf8;
typedef u32 utf32;

typedef u32 b32;
#define TRUE 1
#define FALSE 0

#define NULL 0

#define U8_MAX 255
#define U16_MAX 65535
#define U32_MAX 4294967295U
#define U64_MAX 18446744073709551615ULL

#define I8_MAX 127
#define I16_MAX 32767
#define I32_MAX 2147483647
#define I64_MAX 9223372036854775807LL

#define I8_MIN -128
#define I16_MIN -32768
#define I32_MIN (-I32_MAX-1)
#define I64_MIN (-I64_MAX-1)

u64 max(u64 const a, u64 const b) {
    return (a > b) ? a : b;
}
