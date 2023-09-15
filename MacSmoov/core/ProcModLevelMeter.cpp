//
//  ProcModLevelMeter.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 7/9/23.
//

#include <math.h>
#include <string>
#include "ProcModLevelMeter.hpp"

namespace fmsmoov {

ProcModLevelMeter::ProcModLevelMeter(const string& _name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps) : ProcessorModule(_name, _f_samp, _n_channels, _n_samps) {
    
    //10 ms time constant
    lpf10ms = new LPFSinglePole(get_f_samp(), 0.010);
    prefiltered = new float[get_n_samps()];
    out_rms_l = -100.0;
    out_rms_r = -100.0;
    out_peak_l = -100.0;
    out_peak_r = -100.0;
    this->set_in_buf(0, new AudioBuf(AudioBufType::REFERENCE, string("MAIN_IN_L"), _n_samps));
    this->set_in_buf(1, new AudioBuf(AudioBufType::REFERENCE, string("MAIN_IN_R"), _n_samps));
    this->set_out_buf(0, new AudioBuf(AudioBufType::REAL, string("LINEAR_OUT_L"), _n_samps));
    this->set_out_buf(1, new AudioBuf(AudioBufType::REAL, string("LINEAR_OUT_R"), _n_samps));
    this->set_out_buf(2, new AudioBuf(AudioBufType::REAL, string("DB_OUT_L"), _n_samps));
    this->set_out_buf(3, new AudioBuf(AudioBufType::REAL, string("DB_OUT_R"), _n_samps));
}

ProcModLevelMeter::~ProcModLevelMeter() {
    delete[] prefiltered;
}

void ProcModLevelMeter::process() {
    rms_l = 0.0f;
    rms_r = 0.0f;
    
    peak_l = 0.0f;
    peak_r = 0.0f;
    
    float* inL = this->get_in_buf(0)->getbuf();
    float* inR = this->get_in_buf(1)->getbuf();
    float* outLinL = this->get_out_buf(0)->getbuf();
    float* outLinR = this->get_out_buf(1)->getbuf();
    float* outDBL = this->get_out_buf(2)->getbuf();
    float* outDBR = this->get_out_buf(3)->getbuf();
    uint32_t n_samps = this->get_n_samps();
    
    for(uint32_t i = 0; i < n_samps; i++) {
        rms_l += powf(inL[i], 2.0f);
        rms_r += powf(inR[i], 2.0f);
        tmp_samp_mag = fabs(inL[i]);
        if(tmp_samp_mag > peak_l) {
            peak_l = tmp_samp_mag;
        }
        
        tmp_samp_mag = fabs(inR[i]);
        if(tmp_samp_mag > peak_r) {
            peak_r = tmp_samp_mag;
        }
        
        outDBL[i] = 20.0f * log10f(inL[i]);
        outDBR[i] = 20.0f * log10f(inR[i]);
    }
    
    out_rms_l = 20.0f * log10f(sqrt(rms_l / (float)(n_samps/2)));
    out_rms_r = 20.0f * log10f(sqrt(rms_r / (float)(n_samps/2)));
    out_peak_l = 20.0f * log10f(peak_l);
    out_peak_r = 20.0f * log10f(peak_r);
    
    //printf("Main Input RMS/Peak (L/R): %f, %f, %f, %f\n", rms_l, rms_r, peak_l, peak_r);
    
    memcpy(outLinL, inL, n_samps * sizeof(float));
    memcpy(outLinR, inR, n_samps * sizeof(float));
}

void ProcModLevelMeter::get_levels_db(float* rmsL, float* rmsR, float* peakL, float* peakR) {
    if(rmsL) {
        *rmsL = out_rms_l;
    }
    if(rmsR) {
        *rmsR = out_rms_r;
    }
    if(peakL) {
        *peakL = out_peak_l;
    }
    if(peakR) {
        *peakR = out_peak_r;
    }
}

}
