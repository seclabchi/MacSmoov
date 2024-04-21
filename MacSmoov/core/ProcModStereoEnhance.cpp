//
//  ProcModStereoEnhance.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 8/14/23.
//

#include "ProcModStereoEnhance.hpp"
#include "sos.h"

namespace fmsmoov {

ProcModStereoEnhance::ProcModStereoEnhance(const string& _name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps) : ProcessorModule (_name, _f_samp, _n_channels, _n_samps) {
    this->set_in_buf(0, new AudioBuf(AudioBufType::REFERENCE, "IN_L", _n_samps));
    this->set_in_buf(1, new AudioBuf(AudioBufType::REFERENCE, "IN_R", _n_samps));
    this->set_out_buf(0, new AudioBuf(AudioBufType::REAL, "OUT_L", _n_samps));
    this->set_out_buf(1, new AudioBuf(AudioBufType::REAL, "OUT_R", _n_samps));
    
    line17 = new float[n_samps];  //output of summer 13, heading for VCA 60
    line19 = new float[n_samps];  //output of summer 15, heading for VCA 23
    line39 = new float[n_samps];  //control signal line for VCAs 23 and 60
    line57 = new float[n_samps];  //output of summer 55, heading for VCA 59
    line61 = new float[n_samps];  //incrementally-enhanced L-R VCA output, heading for summers 5 and 7
}

ProcModStereoEnhance::~ProcModStereoEnhance() {
    
}

bool ProcModStereoEnhance::init_impl(CoreConfig* cfg, ProcessorModule* prev_mod, ChannelMap* _channel_map) {
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

void ProcModStereoEnhance::process() {
    if(this->bypass) {
        memcpy(this->outbufs[0]->getbuf(), this->inbufs[0]->getbuf(), n_samps * sizeof(float));
        memcpy(this->outbufs[1]->getbuf(), this->inbufs[1]->getbuf(), n_samps * sizeof(float));
        return;
    }
    
    inL = this->inbufs[0]->getbuf();
    inR = this->inbufs[1]->getbuf();
    outL = this->outbufs[0]->getbuf();
    outR = this->outbufs[1]->getbuf();
    
    for(uint32_t i = 0; i < n_samps; i++) {
        line57[i] = inL[i] - inR[i];
        
        
        outL[i] = inL[i] + line61[i];
        outR[i] = inR[i] - line61[i];
        line17[i] = outL[i] - outR[i];
        line19[i] = outL[i] + outR[i];
    }
    
    //comp59->process(this->inbufs[0]->getbuf(), this->inbufs[1]->getbuf(),
    //                          this->outbufs[0]->getbuf(), this->outbufs[1]->getbuf(), n_samps);
    
    //memcpy(this->get_out_buf(0)->getbuf(), this->get_in_buf(0)->getbuf(), n_samps * sizeof(float));
    //memcpy(this->get_out_buf(1)->getbuf(), this->get_in_buf(1)->getbuf(), n_samps * sizeof(float));
    
}

//VCA 23 is a "decilinear" VCA, gain in dB controlled by input signal
void ProcModStereoEnhance::do_vca_23(float* in, float* control, float* out, uint32_t n_samps) {
    float indb;
    for(uint32_t i = 0; i < n_samps; i++) {
        indb = 20.0f*log10(in[i]);
        out[i] = powf(10.0f, (indb * control[i])/20.0f);
    }
}

//VCA 60 is a "decilinear" VCA, gain in dB controlled by input signal
void ProcModStereoEnhance::do_vca_60(float* in, float* control, float* out, uint32_t n_samps) {
    float indb;
    for(uint32_t i = 0; i < n_samps; i++) {
        indb = 20.0f*log10(in[i]);
        out[i] = powf(10.0f, (indb * control[i])/20.0f);
    }
}

void ProcModStereoEnhance::do_summer_add(float* in1, float* in2, float* out, uint32_t n_samps) {
    for(uint32_t i = 0; i < n_samps; i++) {
        out[i] = in1[i] + in2[i];
    }
}

void ProcModStereoEnhance::do_summer_subtract(float* in1, float* in2, float* out, uint32_t n_samps) {
    for(uint32_t i = 0; i < n_samps; i++) {
        out[i] = in1[i] - in2[i];
    }
}

}
