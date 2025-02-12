//
//  ProcModInputHPF.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 2/9/25.
//

#include "ProcModInputHPF.hpp"

using namespace std;

namespace fmsmoov {

ProcModInputHPF::ProcModInputHPF(const string& _name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps) : ProcessorModule (_name, _f_samp, _n_channels, _n_samps) {

    filt_L = new tonekids::dsp::FilterLR4(_f_samp, 30.0, tonekids::dsp::BUTTERWORTH_TYPE::kHiPass, _n_samps);
    filt_R = new tonekids::dsp::FilterLR4(_f_samp, 30.0, tonekids::dsp::BUTTERWORTH_TYPE::kHiPass, _n_samps);
    
    this->set_in_buf(0, new AudioBuf(AudioBufType::REFERENCE, string("INPUT_HPF_IN_L"), _n_samps));
    this->set_in_buf(1, new AudioBuf(AudioBufType::REFERENCE, string("INPUT_HPF_IN_R"), _n_samps));
    this->set_out_buf(0, new AudioBuf(AudioBufType::REAL, string("INPUT_HPF_OUT_L"), _n_samps));
    this->set_out_buf(1, new AudioBuf(AudioBufType::REAL, string("INPUT_HPF_OUT_R"), _n_samps));
    
}

ProcModInputHPF::~ProcModInputHPF() {
    delete filt_L;
    delete filt_R;
}

bool ProcModInputHPF::init_impl(CoreConfig* cfg, ProcessorModule* prev_mod, ChannelMap* _channel_map) {
    if(nullptr != cfg) {
    }
    if((nullptr != _channel_map) && (nullptr != prev_mod)) {
        for(CHANNEL_MAP_ELEMENT e : _channel_map->the_map)
        {
            this->set_in_buf(e.this_chan, prev_mod->get_out_buf(e.in_chan));
        }
    }
    
    this->set_bypass(false);
    
    this->ready = true;
    
    return true;
}

void ProcModInputHPF::process() {
    uint32_t n_samps = this->get_n_samps();
    
    if(this->get_bypass()) {
        memcpy(this->get_out_buf(0)->getbuf(), this->get_in_buf(0)->getbuf(), n_samps * sizeof(float));
        memcpy(this->get_out_buf(1)->getbuf(), this->get_in_buf(1)->getbuf(), n_samps * sizeof(float));
        return;
    }
    
    filt_L->process(this->get_in_buf(0)->getbuf(), this->get_out_buf(0)->getbuf());
    filt_R->process(this->get_in_buf(1)->getbuf(), this->get_out_buf(1)->getbuf());
}

}
