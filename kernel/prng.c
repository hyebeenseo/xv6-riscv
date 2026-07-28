#include "types.h"

uint32 rs = 123456789;

uint32 xorshift32(void) {
    rs ^= rs << 13;
    rs ^= rs >> 17;
    rs ^= rs << 5;
    return rs;
}

