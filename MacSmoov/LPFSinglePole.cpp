//
//  LPFSinglePole.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 7/11/23.
//

#include "LPFSinglePole.hpp"
#include <math.h>

LPFSinglePole::LPFSinglePole(uint32_t _f_samp, float _tau) : f_samp(_f_samp), rc(_tau) {
    t_samp = 1.0 / (float) f_samp;
    yprevL = 0.0;
    yprevR = 0.0;
    alpha = t_samp / (rc + t_samp);
}

LPFSinglePole::~LPFSinglePole() {
    
}

void LPFSinglePole::process(float* in, float* out, uint32_t n_samp) {
    for(uint32_t i = 0; i < n_samp; i += 2) {
        out[i] = yprevL + alpha * (fabs(in[i]) - yprevL);
        yprevL = out[i];
        out[i+1] = yprevR + alpha * (fabs(in[i+1]) - yprevR);
        yprevR = out[i+1];
    }
}
