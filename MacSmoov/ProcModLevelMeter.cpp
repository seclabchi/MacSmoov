//
//  ProcModLevelMeter.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 7/9/23.
//

#include <math.h>
#include <string>
#include "ProcModLevelMeter.hpp"

namespace fmsmoov {

ProcModLevelMeter::ProcModLevelMeter(uint32_t _f_samp, uint8_t _n_channels, uint32_t _buf_size) : ProcessorModule(_f_samp, _n_channels, _buf_size) {
    
    //10 ms time constant
    lpf10ms = new LPFSinglePole(get_f_samp(), 0.010);
    prefiltered = new float[get_buf_size()];
}

ProcModLevelMeter::~ProcModLevelMeter() {
    delete[] prefiltered;
}

void ProcModLevelMeter::process(float* in, float* out, uint32_t n_samps) {
    rms_l = 0.0;
    rms_r = 0.0;
    
    peak_l = 0.0;
    peak_r = 0.0;
    
    for(uint32_t i = 0; i < n_samps; i+=2) {
        rms_l += powf(in[i], 2.0);
        rms_r += powf(in[i+1], 2.0);
        tmp_samp_mag = fabs(in[i]);
        if(tmp_samp_mag > peak_r) {
            peak_l = tmp_samp_mag;
        }
        
        tmp_samp_mag = fabs(in[i+1]);
        if(tmp_samp_mag > peak_r) {
            peak_r = tmp_samp_mag;
        }
    }
    
    rms_l = 20 * log10(sqrt(rms_l / (float)(n_samps/2)));
    rms_r = 20 * log10(sqrt(rms_r / (float)(n_samps/2)));
    peak_l = 20 * log10(peak_l);
    peak_r = 20 * log10(peak_r);
    
    printf("Main Input RMS/Peak (L/R): %f, %f, %f, %f\n", rms_l, rms_r, peak_l, peak_r);
    
    memcpy(out, in, n_samps * sizeof(float));
    
}

}
