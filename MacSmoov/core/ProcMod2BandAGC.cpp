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
    
    post_drive_bufL = new AudioBuf(AudioBufType::REAL, "AGC_POST_DRIVE_L", _n_samps);
    post_drive_bufR = new AudioBuf(AudioBufType::REAL, "AGC_POST_DRIVE_R", _n_samps);

    filt_lo_L = new tonekids::dsp::FilterLR4(48000.0f, 150.0f, tonekids::dsp::BUTTERWORTH_TYPE::kLoPass, _n_samps);
    filt_lo_R = new tonekids::dsp::FilterLR4(48000.0f, 150.0f, tonekids::dsp::BUTTERWORTH_TYPE::kLoPass, _n_samps);
    filt_hi_L = new tonekids::dsp::FilterLR4(48000.0f, 150.0f, tonekids::dsp::BUTTERWORTH_TYPE::kHiPass, _n_samps);
    filt_hi_R = new tonekids::dsp::FilterLR4(48000.0f, 150.0f, tonekids::dsp::BUTTERWORTH_TYPE::kHiPass, _n_samps);
    
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
    
    comp_lo = new Compressor(this->get_f_samp(), this->get_n_samps());
    comp_hi = new Compressor(this->get_f_samp(), this->get_n_samps());
    
    comp_hi_gain_reduction_buf = new float[_n_samps]();
    memset(comp_hi_gain_reduction_buf, 0, n_samps*sizeof(float));
    
    master_outL = new float[_n_samps]();
    memset(master_outL, 0, _n_samps*sizeof(float));
    master_outR = new float[_n_samps]();
    memset(master_outR, 0, _n_samps*sizeof(float));
    
    this->set_out_buf(0, new AudioBuf(AudioBufType::REAL, "OUT_L", _n_samps));
    this->set_out_buf(1, new AudioBuf(AudioBufType::REAL, "OUT_R", _n_samps));
    
    drive = 0.0;
    
}

ProcMod2BandAGC::~ProcMod2BandAGC() {
    delete post_drive_bufL;
    delete post_drive_bufR;
    
    delete gain_mod;
    
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
        this->set_bypass(!cfg->get_2band_agc_enabled());
        this->configure(params);
    }
    if((nullptr != _channel_map) && (nullptr != prev_mod)) {
        for(CHANNEL_MAP_ELEMENT e : _channel_map->the_map)
        {
            this->set_in_buf(e.this_chan, prev_mod->get_out_buf(e.in_chan));
        }
    }
    
    gain_mod = new ProcModGain("AGC DRIVE", this->f_samp, this->n_channels, this->n_samps);
    gain_mod->init(NULL, NULL, NULL);
    gain_mod->init_manual(this->get_in_buf(0), this->get_in_buf(1), this->post_drive_bufL, this->post_drive_bufR);
    gain_mod->set_gain_db(params.drive, params.drive);
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
    gain_mod->set_bypass(true);
    gain_mod->process();
    
    /*
     * 150 Hz Crossover
     */
    
    filt_lo_L->process(this->post_drive_bufL->getbuf(), buf_lo_filtL);
    filt_lo_R->process(this->post_drive_bufL->getbuf(), buf_lo_filtR);
    filt_hi_L->process(this->post_drive_bufR->getbuf(), buf_hi_filtL);
    filt_hi_R->process(this->post_drive_bufR->getbuf(), buf_hi_filtR);
    
    comp_hi->process(buf_hi_filtL, buf_hi_filtR, agc_out_hiL, agc_out_hiR, n_samps, comp_hi_gain_reduction_buf);
    comp_lo->process(buf_lo_filtL, buf_lo_filtR, agc_out_loL, agc_out_loR, n_samps, NULL, comp_hi_gain_reduction_buf);
    
    for(uint32_t i = 0; i < n_samps; i++) {
        master_outL[i] = agc_out_loL[i] + agc_out_hiL[i];
        master_outR[i] = agc_out_loR[i] + agc_out_hiR[i];
    }
    
    memcpy(this->get_out_buf(0)->getbuf(), master_outL, n_samps * sizeof(float));
    memcpy(this->get_out_buf(1)->getbuf(), master_outR, n_samps * sizeof(float));
}

void ProcMod2BandAGC::configure(const AGC_PARAMS& _params) {
    params = _params;
    

    /*
    float target; //target value dB
    float attack;
    float release;
    float ratio;
    float gate_thresh;
    bool use_coupling;
    float coupling;
    bool use_window;
    float window_size;
    float window_release;
    bool use_idle_gain;
    float idle_gain;
    bool use_post_gain;
    float post_gain;
    float forced_gain_attack;
    float forced_gain_release;
    */
    
/*    DYNAMICS_PROCESSOR_PARAMS comp_parms_lo = {
        .target = _params.target,
        .attack = _params.attack_bass,
        .release = _params.release_bass,
        .ratio = _params.ratio,
        .use_gating = true,
        .gate_thresh = _params.gate_thresh,
        .use_coupling = true,
        .coupling = _params.bass_coupling,
        .use_window = true,
        .window_size = _params.window_size,
        .window_release = _params.window_release,
        .use_idle_gain = true,
        .idle_gain = _params.idle_gain,
        .use_post_gain = false,
        .forced_gain_attack = 1.0f,
        .forced_gain_release = 1.0f
    };
    
    DYNAMICS_PROCESSOR_PARAMS comp_parms_hi = {
        .target = _params.target,
        .attack = _params.attack_master,
        .release = _params.release_master,
        .ratio = _params.ratio,
        .use_gating = true,
        .gate_thresh = _params.gate_thresh,
        .use_coupling = false,
        .coupling = 0.0f,
        .use_window = true,
        .window_size = _params.window_size,
        .window_release = _params.window_release,
        .use_idle_gain = true,
        .idle_gain = _params.idle_gain,
        .use_post_gain = false,
        .forced_gain_attack = 0.0f,
        .forced_gain_release = 0.0f
    };
 */
    COMPRESSOR_PARAMS comp_parms_lo = {
        .target = _params.target,
        .release = _params.release_bass,
        .thresh = _params.gate_thresh,
        .ratio = _params.ratio,
        .attack = _params.attack_bass
    };
    
    COMPRESSOR_PARAMS comp_parms_hi = {
        .target = _params.target,
        .release = _params.release_master,
        .thresh = _params.gate_thresh,
        .ratio = _params.ratio,
        .attack = _params.attack_master
    };

    comp_lo->config(comp_parms_lo);
    comp_hi->config(comp_parms_hi);
}

void ProcMod2BandAGC::read(float* _gain_reduction_lo, float* _gain_reduction_hi, bool* _gate_open_lo, bool* _gate_open_hi) {
    comp_lo->read(_gain_reduction_lo, _gate_open_lo);
    comp_hi->read(_gain_reduction_hi, _gate_open_hi);
    
}

} //namespace fmsmoov
