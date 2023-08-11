//
//  ProcModGain.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 8/11/23.
//

#include <math.h>
#include <string>
#include "ProcModGain.hpp"

namespace fmsmoov {

ProcModGain::ProcModGain(uint32_t _f_samp, uint8_t _n_channels, uint32_t _buf_size) : ProcessorModule(_f_samp, _n_channels, _buf_size) {
    this->set_gain_db(0.0, 0.0);
}

ProcModGain::~ProcModGain() {
}

void ProcModGain::process(float* in, float* out, uint32_t n_samps) {
    for(uint32_t i = 0; i < n_samps; i+=2) {
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
