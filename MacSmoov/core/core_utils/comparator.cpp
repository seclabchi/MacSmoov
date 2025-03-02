//
//  comparator.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 3/1/25.
//

#include "comparator.hpp"

#include <cmath>

namespace fmsmoov {

Comparator::Comparator(uint32_t _f_samp, uint32_t _n_samps) {
    thresh_lo = 0.0;
    thresh_hi = 0.0;
}

Comparator::~Comparator() {
    
}

void Comparator::configure(COMPARATOR_TYPE _type, float _thresh_lo, float _thresh_hi) {
    
    /*
     * If type is COMPARATOR_TYPE::SINGLE_THRESH, then only the thresh_lo value will
     * be used in the processing.
     */
    
    type = _type;
    thresh_lo = _thresh_lo;
    thresh_hi = _thresh_hi;
}

void Comparator::process(float* in1, float* out, float* in2) {
    
    /*
     * If comparator type is COMPARATOR_TYPE::SINGLE_THRESH, then the
     * in2 values are ignored.
     */
    
    for(uint32_t i = 0; i < n_samps; i++) {
        
        if(type == COMPARATOR_TYPE::WINDOWED) {
            if((fabs(in1[i] / (in2[i] + EPS)) < thresh_lo) || (fabs(in1[i] / (in2[i] + EPS)) > thresh_hi)) {
                out[i] = 0.0f;
            }
            else {
                out[i] = 1.0f;
            }
        }
        else {
            if(fabs(in1[i]) > thresh_lo) {
                out[i] = 1.0;
            }
            else {
                out[i] = 0.0;
            }
        }
    }
}

}  //namespace fmsmoov
