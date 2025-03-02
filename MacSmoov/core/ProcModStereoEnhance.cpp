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
    
    line17 = new float[n_samps]();
    vca_ctrl_39 = new float[n_samps]();
    comp_enh_l_minus_r = new float[n_samps]();
    bpf62_out = new float[n_samps]();
    comparator65_out = new float[n_samps]();
    
    vca60 = new VCA(f_samp, n_samps);
    vca60->config(VCA_TYPE::LINEAR);
    
    bpf62 = new tonekids::dsp::Butterworth();
    float fc1 = 100.0f;
    float fc2 = 2000.0f;
    
    comparator65 = new Comparator(f_samp, n_samps);
    comparator65->configure(COMPARATOR_TYPE::WINDOWED, 0.1, 0.8);

    double bpf62_bq_gain = 0.0;
    bpf62->bandPass((double)f_samp, (double)fc1, (double)fc2, (int)2, bpf62_bq, bpf62_bq_gain);

}

ProcModStereoEnhance::~ProcModStereoEnhance() {
    delete[] line17;
    delete[] vca_ctrl_39;
    delete[] comp_enh_l_minus_r;
    delete[] bpf62_out;
    delete[] comparator65_out;
    
    delete vca60;
    delete bpf62;
    delete comparator65;
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
    
    //drive = cfg->get_stereo_enhance_drive();
    avg_lr_diff = 0.0f;
    this->ready = true;
    return true;
}

void ProcModStereoEnhance::configure() {
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
    
    /*
    subtract_buffers(inL, inR, line17);
    vca60->process(line17, vca_ctrl_39, comp_enh_l_minus_r);
    //bpf62_bq[0].process(comp_enh_l_minus_r, bpf62_out, n_samps);
    comparator65->process(comp_enh_l_minus_r, comparator65_out, comp_enh_l_minus_r);
    */
    memcpy(this->outbufs[0]->getbuf(), this->inbufs[0]->getbuf(), n_samps * sizeof(float));
    memcpy(this->outbufs[1]->getbuf(), this->inbufs[1]->getbuf(), n_samps * sizeof(float));
    
    avg_lr_diff = avg_lr_diff / n_samps;
}

void ProcModStereoEnhance::read(float* _lrdiff) {
    if(_lrdiff) {
        *_lrdiff = avg_lr_diff;
    }
}

void ProcModStereoEnhance::subtract_buffers(float* buf1, float* buf2, float* out) {
    for(uint32_t i = 0; i < n_samps; i++) {
        out[i] = buf1[i] - buf2[i];
    }
}

void ProcModStereoEnhance::add_buffers(float* buf1, float* buf2, float* out) {
    for(uint32_t i = 0; i < n_samps; i++) {
        out[i] = buf1[i] + buf2[i];
    }
}
    
}
