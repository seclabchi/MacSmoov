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
    buf_main_in = new AudioBuf(AudioBufType::REFERENCE, string("MAIN_IN"), _n_samps);
    buf_lin_out = new AudioBuf(AudioBufType::REAL, string("LINEAR_OUT"), _n_samps);
    buf_db_out = new AudioBuf(AudioBufType::REAL, string("DB_OUT"), _n_samps);
    this->set_in_buf(0, buf_main_in);
    this->set_out_buf(0, buf_lin_out);
    this->set_out_buf(1, buf_db_out);
}

ProcModLevelMeter::~ProcModLevelMeter() {
    delete[] prefiltered;
    delete buf_main_in;
    delete buf_lin_out;
    delete buf_db_out;
}

void ProcModLevelMeter::process() {
    rms_l = 0.0;
    rms_r = 0.0;
    
    peak_l = 0.0;
    peak_r = 0.0;
    
    float* in = this->get_in_buf(0)->getbuf();
    float* outLin = this->get_out_buf(0)->getbuf();
    float* outDB = this->get_out_buf(1)->getbuf();
    uint32_t n_samps = this->get_n_samps();
    
    for(uint32_t i = 0; i < n_samps; i+=2) {
        rms_l += powf(in[i], 2.0);
        rms_r += powf(in[i+1], 2.0);
        tmp_samp_mag = fabs(in[i]);
        if(tmp_samp_mag > peak_r) {
            peak_l = tmp_samp_mag;
        }
        
        tmp_samp_mag = fabs(in[i+1]);
        if(tmp_samp_mag > peak_r) {
            peak_r = tmp_samp_mag;
        }
        
        outDB[i] = 20 * log10(in[i]);
    }
    
    out_rms_l = 20 * log10(sqrt(rms_l / (float)(n_samps/2)));
    out_rms_r = 20 * log10(sqrt(rms_r / (float)(n_samps/2)));
    out_peak_l = 20 * log10(peak_l);
    out_peak_r = 20 * log10(peak_r);
    
    //printf("Main Input RMS/Peak (L/R): %f, %f, %f, %f\n", rms_l, rms_r, peak_l, peak_r);
    
    memcpy(outLin, in, n_samps * sizeof(float));
    
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
