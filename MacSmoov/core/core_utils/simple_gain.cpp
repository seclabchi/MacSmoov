//
//  simple_gain.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 1/23/25.
//

#include "simple_gain.hpp"

#include <cstring>

namespace fmsmoov {

SimpleGain::SimpleGain(float _init_gain, uint32_t _n_samp) : gain(_init_gain), n_samp(_n_samp) {

}

SimpleGain::~SimpleGain() {

}

void SimpleGain::process(float* inL, float* inR, float* outL, float* outR) {
    
    for(uint32_t i = 0; i < n_samp; i++) {
        outL[i] = inL[i] * powf(10.0f, gain/20.0f);
        outR[i] = inR[i] * powf(10.0f, gain/20.0f);
    }
    
}

void SimpleGain::set_gain(float new_gain) {
    gain = new_gain;
}

float SimpleGain::get_gain() {
    return gain;
}

} //namespace FMSMOOV
