//
//  ProcModGain.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 8/11/23.
//

#include <math.h>
#include "ProcModDelay.hpp"

namespace fmsmoov {

ProcModDelay::ProcModDelay(const string& _name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps) : ProcessorModule(_name, _f_samp, _n_channels, _n_samps) {
    this->set_delay(0.0); //ms
    set_in_buf(0, new AudioBuf(AudioBufType::REFERENCE, string("IN_L"), _n_samps, NULL));
    set_in_buf(1, new AudioBuf(AudioBufType::REFERENCE, string("IN_R"), _n_samps, NULL));
    set_out_buf(0, new AudioBuf(AudioBufType::REAL, string("OUT_L"), _n_samps, NULL));
    set_out_buf(1, new AudioBuf(AudioBufType::REAL, string("OUT_R"), _n_samps, NULL));
}

ProcModDelay::~ProcModDelay() {
    
}

void ProcModDelay::process() {
    float* inL = this->get_in_buf(0)->getbuf();
    float* inR = this->get_in_buf(1)->getbuf();
    float* outL = this->get_out_buf(0)->getbuf();
    float* outR = this->get_out_buf(1)->getbuf();
    uint32_t n_samps = this->get_n_samps();
    
    for(uint32_t i = 0; i < n_samps; i++) {
        
    }
}

void ProcModDelay::set_delay(float _delay_ms) {
    m_delay_ms = _delay_ms;
    m_delay_samp = 0;
    m_bufL = new float[this->get_n_samps()];
    m_bufR = new float[this->get_n_samps()];
}

}
