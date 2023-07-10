//
//  ProcModLevelMeter.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 7/9/23.
//

#include <math.h>
#include "ProcModLevelMeter.hpp"

namespace fmsmoov {

ProcModLevelMeter::ProcModLevelMeter(uint32_t _f_samp, uint8_t _n_channels) : ProcessorModule(_f_samp, _n_channels) {
    
}

ProcModLevelMeter::~ProcModLevelMeter() {
    
}

void ProcModLevelMeter::process(float* inL, float* inR, float* outL, float* outR, uint32_t nframes) {
    rms_l = 0.0;
    rms_r = 0.0;
    
    for(uint32_t i = 0; i < nframes; i++) {
        rms_l += powf(inL[i], 2.0);
        rms_r += powf(inR[i], 2.0);
    }
    
    rms_l = 20 * log10(sqrt(rms_l / nframes));
    rms_r = 20 * log10(sqrt(rms_r / nframes));
    
    printf("Main Input RMS: %f, %f", rms_l, rms_r);
}

}
