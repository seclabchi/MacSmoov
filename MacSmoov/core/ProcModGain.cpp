//
//  ProcModGain.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 8/11/23.
//

#include <math.h>
#include "ProcModGain.hpp"

namespace fmsmoov {

ProcModGain::ProcModGain(const string& _name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps) : ProcessorModule(_name, _f_samp, _n_channels, _n_samps) {
    this->set_gain_db(0.0, 0.0);
    set_in_buf(0, new AudioBuf(AudioBufType::REFERENCE, string("IN_L"), _n_samps, NULL));
    set_in_buf(1, new AudioBuf(AudioBufType::REFERENCE, string("IN_R"), _n_samps, NULL));
    set_out_buf(0, new AudioBuf(AudioBufType::REAL, string("OUT_L"), _n_samps, NULL));
    set_out_buf(1, new AudioBuf(AudioBufType::REAL, string("OUT_R"), _n_samps, NULL));
}

ProcModGain::~ProcModGain() {
    delete this->get_in_buf(0);
    delete this->get_out_buf(0);
    delete this->get_in_buf(1);
    delete this->get_out_buf(1);
}

void ProcModGain::process() {
    float* inL = this->get_in_buf(0)->getbuf();
    float* inR = this->get_in_buf(1)->getbuf();
    float* outL = this->get_out_buf(0)->getbuf();
    float* outR = this->get_out_buf(1)->getbuf();
    uint32_t n_samps = this->get_n_samps();
    
    for(uint32_t i = 0; i < n_samps; i++) {
        outL[i] = inL[i] * m_setgain_lin_L;
        outR[i] = inR[i] * m_setgain_lin_R;
    }
}

void ProcModGain::set_gain_db(float _gainL, float _gainR) {
    m_setgain_db_L = _gainL;
    m_setgain_lin_L = powf(10, _gainL/20.0);
    m_setgain_db_R = _gainR;
    m_setgain_lin_R = powf(10, _gainR/20.0);
}

}
