//
//  ProcModLookaheadLimiter.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 9/20/24.
//

#include <math.h>
#include "ProcModLookaheadLimiter.h"

namespace fmsmoov {

ProcModLookaheadLimiter::ProcModLookaheadLimiter(const string& _name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps) : ProcessorModule(_name, _f_samp, _n_channels, _n_samps) {
    this->set_gain_db(0.0, 0.0);
    set_in_buf(0, new AudioBuf(AudioBufType::REFERENCE, string("IN_L"), _n_samps, NULL));
    set_in_buf(1, new AudioBuf(AudioBufType::REFERENCE, string("IN_R"), _n_samps, NULL));
    set_out_buf(0, new AudioBuf(AudioBufType::REAL, string("OUT_L"), _n_samps, NULL));
    set_out_buf(1, new AudioBuf(AudioBufType::REAL, string("OUT_R"), _n_samps, NULL));
}

ProcModLookaheadLimiter::~ProcModLookaheadLimiter() {
    delete this->get_in_buf(0);
    delete this->get_out_buf(0);
    delete this->get_in_buf(1);
    delete this->get_out_buf(1);
}

bool ProcModLookaheadLimiter::init_impl(CoreConfig* cfg, ProcessorModule* prev_mod, ChannelMap* _channel_map) {
    if(nullptr != cfg) {
        //TODO config
    }
    if((nullptr != _channel_map) && (nullptr != prev_mod)) {
        for(CHANNEL_MAP_ELEMENT e : _channel_map->the_map)
        {
            this->set_in_buf(e.this_chan, prev_mod->get_out_buf(e.in_chan));
        }
    }
    
    return true;
}

void ProcModLookaheadLimiter::set_gain_db(float gain_L, float gain_R) {
    
}

void ProcModLookaheadLimiter::process() {
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

}
