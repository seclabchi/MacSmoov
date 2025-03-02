//
//  ProcMod2BandAGC.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 8/12/23.
//

#include "ProcMod2BandAGC.hpp"
#include <iostream>

using namespace std;

namespace fmsmoov {

ProcMod2BandAGC::ProcMod2BandAGC(const string& _name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps) : ProcessorModule (_name, _f_samp, _n_channels, _n_samps) {
    
    agc_drive = new SimpleGain(0.0, _n_samps);
    post_drive_bufL = new AudioBuf(AudioBufType::REAL, "AGC_DRIVE_L", _n_samps, NULL);
    post_drive_bufR = new AudioBuf(AudioBufType::REAL, "AGC_DRIVE_R", _n_samps, NULL);
    
    filt_lo_L = new tonekids::dsp::FilterLR4(48000.0f, 200.0f, tonekids::dsp::BUTTERWORTH_TYPE::kLoPass, _n_samps);
    filt_lo_R = new tonekids::dsp::FilterLR4(48000.0f, 200.0f, tonekids::dsp::BUTTERWORTH_TYPE::kLoPass, _n_samps);
    filt_hi_L = new tonekids::dsp::FilterLR4(48000.0f, 200.0f, tonekids::dsp::BUTTERWORTH_TYPE::kHiPass, _n_samps);
    filt_hi_R = new tonekids::dsp::FilterLR4(48000.0f, 200.0f, tonekids::dsp::BUTTERWORTH_TYPE::kHiPass, _n_samps);
    
    buf_lo_filtL = new float[_n_samps]();
    memset(buf_lo_filtL, 0, _n_samps*sizeof(float));
    buf_lo_filtR = new float[_n_samps]();
    memset(buf_lo_filtR, 0, _n_samps*sizeof(float));
    buf_hi_filtL = new float[_n_samps]();
    memset(buf_hi_filtL, 0, _n_samps*sizeof(float));
    buf_hi_filtR = new float[_n_samps]();
    memset(buf_hi_filtR, 0, _n_samps*sizeof(float));
    
    agc_out_loL = new float[_n_samps]();
    memset(agc_out_loL, 0, _n_samps*sizeof(float));
    agc_out_hiL = new float[_n_samps]();
    memset(agc_out_hiL, 0, _n_samps*sizeof(float));
    agc_out_loR = new float[_n_samps]();
    memset(agc_out_loR, 0, _n_samps*sizeof(float));
    agc_out_hiR = new float[_n_samps]();
    memset(agc_out_hiR, 0, _n_samps*sizeof(float));
    
    gc_hi = new float[_n_samps]();
    
    comp_lo = new Compressor(this->get_f_samp(), this->get_n_samps(), "2 BAND AGC LO");
    comp_hi = new Compressor(this->get_f_samp(), this->get_n_samps(), "2 BAND AGC HI");
    
    comp_hi_gain_reduction_buf = new float[_n_samps]();
    memset(comp_hi_gain_reduction_buf, 0, n_samps*sizeof(float));
    
    master_outL = new float[_n_samps]();
    memset(master_outL, 0, _n_samps*sizeof(float));
    master_outR = new float[_n_samps]();
    memset(master_outR, 0, _n_samps*sizeof(float));
    
    this->set_in_buf(0, new AudioBuf(AudioBufType::REFERENCE, "IN_L", _n_samps));
    this->set_in_buf(1, new AudioBuf(AudioBufType::REFERENCE, "IN_R", _n_samps));
    this->set_out_buf(0, new AudioBuf(AudioBufType::REAL, "OUT_L", _n_samps));
    this->set_out_buf(1, new AudioBuf(AudioBufType::REAL, "OUT_R", _n_samps));
    
    drive = 0.0;
    
}

ProcMod2BandAGC::~ProcMod2BandAGC() {
    delete agc_drive;
    
    delete post_drive_bufL;
    delete post_drive_bufR;
    
    delete[] gc_hi;
        
    delete filt_lo_L;
    delete filt_lo_R;
    delete filt_hi_L;
    delete filt_hi_R;

    delete[] buf_lo_filtL;
    delete[] buf_lo_filtR;
    delete[] buf_hi_filtL;
    delete[] buf_hi_filtR;
    
    delete[] agc_out_loL;
    delete[] agc_out_hiL;
    delete[] agc_out_loR;
    delete[] agc_out_hiR;
    
    delete[] master_outL;
    delete[] master_outR;
    
    delete[] comp_hi_gain_reduction_buf;
}

bool ProcMod2BandAGC::init_impl(CoreConfig* cfg, ProcessorModule* prev_mod, ChannelMap* _channel_map) {
    if(nullptr != cfg) {
        AGC_PARAMS params;
        cfg->get_agc_params(params);
        this->configure(params);
    }
    if((nullptr != _channel_map) && (nullptr != prev_mod)) {
        for(CHANNEL_MAP_ELEMENT e : _channel_map->the_map)
        {
            this->set_in_buf(e.this_chan, prev_mod->get_out_buf(e.in_chan));
        }
    }
    
    this->ready = true;
    
    return true;
}

void ProcMod2BandAGC::process() {
    uint32_t n_samps = this->get_n_samps();
    
    if(this->get_bypass()) {
        memcpy(this->get_out_buf(0)->getbuf(), this->get_in_buf(0)->getbuf(), n_samps * sizeof(float));
        memcpy(this->get_out_buf(1)->getbuf(), this->get_in_buf(1)->getbuf(), n_samps * sizeof(float));
        return;
    }
    
    /*
     * Apply drive
     */
    
    agc_drive->process(this->get_in_buf(0)->getbuf(), this->get_in_buf(1)->getbuf(), this->post_drive_bufL->getbuf(), this->post_drive_bufR->getbuf());
    
    
    /*
     * 150 Hz Crossover
     */
    
    filt_lo_L->process(this->post_drive_bufL->getbuf(), buf_lo_filtL);
    filt_lo_R->process(this->post_drive_bufR->getbuf(), buf_lo_filtR);
    filt_hi_L->process(this->post_drive_bufL->getbuf(), buf_hi_filtL);
    filt_hi_R->process(this->post_drive_bufR->getbuf(), buf_hi_filtR);
    
    /* If we want to mute the high or low side of the AGC output, still do the processing so the bass coupling will still work */
    
    comp_hi->compute_gc(buf_hi_filtL, buf_hi_filtR, gc_hi);
    comp_hi->process(buf_hi_filtL, buf_hi_filtR, agc_out_hiL, agc_out_hiR, n_samps, NULL, comp_hi_gain_reduction_buf);
    
    if(params.mute_hi) {
        memset(agc_out_hiL, 0, n_samps * sizeof(float));
        memset(agc_out_hiR, 0, n_samps * sizeof(float));
    }
    
    comp_lo->compute_gc(buf_lo_filtL, buf_lo_filtR);
    comp_lo->process(buf_lo_filtL, buf_lo_filtR, agc_out_loL, agc_out_loR, n_samps, gc_hi, comp_hi_gain_reduction_buf);
    
    if(params.mute_lo) {
        memset(agc_out_loL, 0, n_samps * sizeof(float));
        memset(agc_out_loR, 0, n_samps * sizeof(float));
    }
    
    
    
    for(uint32_t i = 0; i < n_samps; i++) {
        master_outL[i] = agc_out_loL[i] + agc_out_hiL[i];
        master_outR[i] = agc_out_loR[i] + agc_out_hiR[i];
    }
    
    memcpy(this->get_out_buf(0)->getbuf(), master_outL, n_samps * sizeof(float));
    memcpy(this->get_out_buf(1)->getbuf(), master_outR, n_samps * sizeof(float));
}

void ProcMod2BandAGC::configure(const AGC_PARAMS& _params) {
    params = _params;
    
    COMPRESSOR_PARAMS comp_parms_lo = {
        .thresh = _params.thresh,
        .release = _params.release_bass,
        .gate_thresh = _params.gate_thresh,
        .ratio = _params.ratio,
        .attack = _params.attack_bass,
        .knee_type = _params.bass_knee_type,
        .knee_width = _params.bass_knee_width,
        .idle_gain = _params.idle_gain,
        .makeup_gain_mode = _params.bass_makeup_gain_mode,
        .makeup_gain = _params.bass_makeup_gain
    };
    
    COMPRESSOR_PARAMS comp_parms_hi = {
        .thresh = _params.thresh,
        .release = _params.release_master,
        .gate_thresh = _params.gate_thresh,
        .ratio = _params.ratio,
        .attack = _params.attack_master,
        .knee_type = _params.master_knee_type,
        .knee_width = _params.master_knee_width,
        .idle_gain = _params.idle_gain,
        .makeup_gain_mode = _params.master_makeup_gain_mode,
        .makeup_gain = _params.master_makeup_gain
    };
    
    bool use_coupling = false;
    
    if(params.bass_coupling > 0.0) {
        use_coupling = true;
    }

    this->set_bypass(!(_params.enabled));
    agc_drive->set_gain(_params.drive);
    comp_lo->config(comp_parms_lo, use_coupling, params.bass_coupling);
    comp_hi->config(comp_parms_hi);
}

void ProcMod2BandAGC::read(float* _gain_reduction_lo, float* _gain_reduction_hi, bool* _gate_open_lo, bool* _gate_open_hi) {
    comp_lo->read(_gain_reduction_lo, _gate_open_lo);
    comp_hi->read(_gain_reduction_hi, _gate_open_hi);
    
}

} //namespace fmsmoov
