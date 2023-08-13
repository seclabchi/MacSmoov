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
    inbuf_main = new AudioBuf(AudioBufType::REFERENCE, string("IN"), _n_samps, NULL);
    this->set_in_buf(0, inbuf_main);
    outbuf_main = new AudioBuf(AudioBufType::REAL, string("OUT"), _n_samps, NULL);
    this->set_out_buf(0, outbuf_main);
}

ProcModGain::~ProcModGain() {
    delete inbuf_main;
    delete outbuf_main;
}

void ProcModGain::process() {
    float* in = this->get_in_buf(0)->getbuf();
    float* out = this->get_out_buf(0)->getbuf();
    
    for(uint32_t i = 0; i < this->get_n_samps(); i+=2) {
        out[i] = in[i] * m_setgain_lin_L;
        out[i+1] = in[i+1] * m_setgain_lin_R;
    }
}

void ProcModGain::set_gain_db(float _gainL, float _gainR) {
    m_setgain_db_L = _gainL;
    m_setgain_lin_L = powf(10, _gainL/20.0);
    m_setgain_db_R = _gainR;
    m_setgain_lin_R = powf(10, _gainR/20.0);
}

}
