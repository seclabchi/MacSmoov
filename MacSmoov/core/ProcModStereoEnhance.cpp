//
//  ProcModStereoEnhance.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 8/14/23.
//

#include "ProcModStereoEnhance.hpp"

namespace fmsmoov {

ProcModStereoEnhance::ProcModStereoEnhance(const string& _name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps) : ProcessorModule (_name, _f_samp, _n_channels, _n_samps) {
    this->set_in_buf(0, new AudioBuf(AudioBufType::REFERENCE, "IN_L", _n_samps));
    this->set_in_buf(1, new AudioBuf(AudioBufType::REFERENCE, "IN_R", _n_samps));
    this->set_out_buf(0, new AudioBuf(AudioBufType::REAL, "OUT_L", _n_samps));
    this->set_out_buf(1, new AudioBuf(AudioBufType::REAL, "OUT_R", _n_samps));
}

ProcModStereoEnhance::~ProcModStereoEnhance() {
    
}

bool ProcModStereoEnhance::init_impl(CoreConfig* cfg, ProcessorModule* prev_mod, ChannelMap* _channel_map) {
    if(nullptr != cfg) {
        this->set_bypass(!cfg->get_stereo_enhance_enabled());
    }
    if((nullptr != _channel_map) && (nullptr != prev_mod)) {
        for(CHANNEL_MAP_ELEMENT e : _channel_map->the_map)
        {
            this->set_in_buf(e.this_chan, prev_mod->get_out_buf(e.in_chan));
        }
    }
    
    drive = cfg->get_stereo_enhance_drive();
    avg_lr_diff = 0.0f;
    
    return true;
}

void ProcModStereoEnhance::configure(float _drive) {
    drive = _drive;
}

void ProcModStereoEnhance::process() {
    if(this->bypass) {
        memcpy(this->outbufs[0]->getbuf(), this->inbufs[0]->getbuf(), n_samps * sizeof(float));
        memcpy(this->outbufs[1]->getbuf(), this->inbufs[1]->getbuf(), n_samps * sizeof(float));
        return;
    }
    
    avg_lr_diff = 0.0f;
    
    inL = this->inbufs[0]->getbuf();
    inR = this->inbufs[1]->getbuf();
    outL = this->outbufs[0]->getbuf();
    outR = this->outbufs[1]->getbuf();
    
    for(size_t i = 0; i < n_samps; i++) {
        M = (inL[i] + inR[i]) / 2.0f;
        S = (inL[i] - inR[i]) * drive;
        avg_lr_diff += fabs(S);
        outL[i] = M + S;
        outR[i] = M - S;
    }
    
    avg_lr_diff = avg_lr_diff / n_samps;
}

void ProcModStereoEnhance::read(float* _lrdiff) {
    if(_lrdiff) {
        *_lrdiff = avg_lr_diff;
    }
}
    
}
