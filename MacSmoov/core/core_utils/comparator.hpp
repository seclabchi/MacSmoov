//
//  comparator.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 3/1/25.
//

#ifndef comparator_hpp
#define comparator_hpp

#include <stdio.h>
#include <stdint.h>

#define EPS 0.00000001

namespace fmsmoov {

typedef enum {
    SINGLE_THRESH = 0,
    WINDOWED = 1
} COMPARATOR_TYPE;

class Comparator {
public:
    Comparator(uint32_t _f_samp, uint32_t _n_samps);
    virtual ~Comparator();
    virtual void configure(COMPARATOR_TYPE _type, float _thresh_lo, float _thresh_hi);
    virtual void process(float* in1, float* out, float* in2 = NULL);
private:
    COMPARATOR_TYPE type;
    uint32_t f_samp;
    uint32_t n_samps;
    float thresh_lo;
    float thresh_hi;
};

} //namespace fmsmoov

#endif /* comparator_hpp */
