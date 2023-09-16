//
//  CircularBuffer.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 7/26/23.
//

#ifndef CircularBuffer_hpp
#define CircularBuffer_hpp

#include <stdio.h>
#include <cstdint>

typedef struct {
    uint8_t * const buffer;
    int head;
    int tail;
    const int maxlen;
} circ_bbuf_t;

#endif /* CircularBuffer_hpp */
