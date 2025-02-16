//
//  ProcModClipper.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 2/9/25.
//

#include "ProcModClipper.hpp"

using namespace std;

namespace fmsmoov {

ProcModClipper::ProcModClipper(const string& _name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps) : ProcessorModule (_name, _f_samp, _n_channels, _n_samps) {

    this->set_in_buf(0, new AudioBuf(AudioBufType::REFERENCE, string("FINAL_LPF_IN_L"), _n_samps));
    this->set_in_buf(1, new AudioBuf(AudioBufType::REFERENCE, string("FINAL_LPF_IN_R"), _n_samps));
    this->set_out_buf(0, new AudioBuf(AudioBufType::REAL, string("FINAL_LPF_OUT_L"), _n_samps));
    this->set_out_buf(1, new AudioBuf(AudioBufType::REAL, string("FINAL_LPF_OUT_R"), _n_samps));
    clip_level = 1.0;
}

ProcModClipper::~ProcModClipper() {

}

bool ProcModClipper::init_impl(CoreConfig* cfg, ProcessorModule* prev_mod, ChannelMap* _channel_map) {
    if(nullptr != cfg) {
    }
    if((nullptr != _channel_map) && (nullptr != prev_mod)) {
        for(CHANNEL_MAP_ELEMENT e : _channel_map->the_map)
        {
            this->set_in_buf(e.this_chan, prev_mod->get_out_buf(e.in_chan));
        }
    }
    
    this->set_bypass(false);
    this->clip_level = cfg->get_clip_level();
    
    this->ready = true;
    
    return true;
}

void ProcModClipper::configure(float _clip_level) {
    clip_level = _clip_level;
}


void ProcModClipper::process() {
    uint32_t n_samps = this->get_n_samps();
    
    if(this->get_bypass()) {
        memcpy(this->get_out_buf(0)->getbuf(), this->get_in_buf(0)->getbuf(), n_samps * sizeof(float));
        memcpy(this->get_out_buf(1)->getbuf(), this->get_in_buf(1)->getbuf(), n_samps * sizeof(float));
        return;
    }
    
    float* inL = this->get_in_buf(0)->getbuf();
    float* inR = this->get_in_buf(1)->getbuf();
    float* outL = this->get_out_buf(0)->getbuf();
    float* outR = this->get_out_buf(1)->getbuf();
    
    for(size_t i = 0; i < n_samps; i++) {
        if(inL[i] < -clip_level) {
            outL[i] = -clip_level;
        }
        else if(inL[i] > clip_level) {
            outL[i] = clip_level;
        }
        else {
            outL[i] = inL[i];
        }
        
        if(inR[i] < -clip_level) {
            outR[i] = -clip_level;
        }
        else if(inR[i] > clip_level) {
            outR[i] = clip_level;
        }
        else {
            outR[i] = inR[i];
        }
    }
    
    
}

}
