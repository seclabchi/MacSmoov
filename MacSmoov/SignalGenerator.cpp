//
//  SignalGenerator.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 4/1/23.
//

#include "SignalGenerator.hpp"

#include <iostream>
#include <math.h>

using namespace fmsmoov;

SignalGenerator::SignalGenerator(uint32_t _f_samp, uint32_t _n_channels) : f_samp(_f_samp), n_channels(_n_channels) {
    volume = 1.0;
}

SignalGenerator::~SignalGenerator() {
    
}

void SignalGenerator::configure(SIG_GEN_TYPE _type, uint32_t _frequency) {
    type = _type;
    frequency = _frequency;
}

void SignalGenerator::get_next_buffers(float* out_buf, uint32_t n_frames) {
    static uint32_t cur_samp = 0;
    float sin_val = 0;
    
    for(uint32_t i = 0; i < n_frames; i+=2) {
        sin_val = volume * sin(2.0f * M_PI * frequency * (float)(cur_samp/f_samp));
        out_buf[i] = sin_val;
        out_buf[i+1] = sin_val;
        if(cur_samp == f_samp) {
            cur_samp = 1.0f;
        }
        else {
            cur_samp++;
        }
    }
    
}

void SignalGenerator::adjust_volume(float _volume) {
    volume = _volume;
}
