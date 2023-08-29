//
//  ProcMod2BandAGC.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 8/12/23.
//

#include "ProcMod2BandAGC.hpp"
#include "sos.h"
#include <iostream>

using namespace std;

namespace fmsmoov {

/* 44100 Hz sample rate, 150 Hz LO/HI pass second order sections
 soslo =

    1.0000   2.0000   1.0000   1.0000  -1.9849   0.9850

 g = 2.8332e-05
 soshi =

    1.0000  -2.0000   1.0000   1.0000  -1.9849   0.9850

 g = 0.9925
 

 48000 Hz sample rate, 150 Hz LO/HI pass second order sections
 
soslo =

   1.0000   2.0000   1.0000   1.0000  -1.9861   0.9862

g = 2.3929e-05
soshi =

   1.0000  -2.0000   1.0000   1.0000  -1.9861   0.9862

g = 0.9931
 
*/

ProcMod2BandAGC::ProcMod2BandAGC(const string& _name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps) : ProcessorModule (_name, _f_samp, _n_channels, _n_samps) {
    SOS soslo(2.3929e-05, 1.0000, 2.0000, 1.0000, 1.0000, -1.9861, 0.9862);
    SOS soshi(0.9931, 1.0000, -2.0000, 1.0000, 1.0000, -1.9861, 0.9862);
    filt_lo_L = new FilterLR4(&soslo, _n_samps);
    filt_lo_R = new FilterLR4(&soslo, _n_samps);
    filt_hi_L = new FilterLR4(&soshi, _n_samps);
    filt_hi_R = new FilterLR4(&soshi, _n_samps);
    
    buf_lo_filtL = new float[_n_samps]();
    buf_lo_filtR = new float[_n_samps]();
    buf_hi_filtL = new float[_n_samps]();
    buf_hi_filtR = new float[_n_samps]();
    
    agc_out_loL = new float[_n_samps]();
    agc_out_hiL = new float[_n_samps]();
    agc_out_loR = new float[_n_samps]();
    agc_out_hiR = new float[_n_samps]();
    
    comp_lo = NULL;
    comp_hi = NULL;
    
    comp_hi_gain_reduction_buf = new float[_n_samps];
    
    master_outL = new float[_n_samps]();
    master_outR = new float[_n_samps]();
    
    this->set_out_buf(0, new AudioBuf(AudioBufType::REAL, "OUT_L", _n_samps));
    this->set_out_buf(1, new AudioBuf(AudioBufType::REAL, "OUT_R", _n_samps));
    
    linlogL = new LogLinConverter(LogLinConversionType::LIN_TO_LOG);
    linlogR = new LogLinConverter(LogLinConversionType::LIN_TO_LOG);
    

}

ProcMod2BandAGC::~ProcMod2BandAGC() {
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
    
    delete[] comp_hi_gain_reduction_buf;
}

void ProcMod2BandAGC::process() {
    uint32_t n_samps = this->get_n_samps();
    
    if(this->get_bypass()) {
        memcpy(this->get_out_buf(0)->getbuf(), this->get_in_buf(0)->getbuf(), n_samps * sizeof(float));
        memcpy(this->get_out_buf(1)->getbuf(), this->get_in_buf(1)->getbuf(), n_samps * sizeof(float));
        return;
    }
    
    filt_lo_L->process(this->get_in_buf(0)->getbuf(), buf_lo_filtL);
    filt_lo_R->process(this->get_in_buf(1)->getbuf(), buf_lo_filtR);
    filt_hi_L->process(this->get_in_buf(0)->getbuf(), buf_hi_filtL);
    filt_hi_R->process(this->get_in_buf(1)->getbuf(), buf_hi_filtR);
    
    comp_hi->process(buf_hi_filtL, buf_hi_filtR, agc_out_hiL, agc_out_hiR, n_samps, comp_hi_gain_reduction_buf);
    comp_lo->process(buf_lo_filtL, buf_lo_filtR, agc_out_loL, agc_out_loR, n_samps, NULL, comp_hi_gain_reduction_buf);
    
    for(uint32_t i = 0; i < n_samps; i++) {
        master_outL[i] = agc_out_loL[i] + agc_out_hiL[i];
        master_outR[i] = agc_out_loR[i] + agc_out_hiR[i];
    }
    
    memcpy(this->get_out_buf(0)->getbuf(), master_outL, n_samps * sizeof(float));
    memcpy(this->get_out_buf(1)->getbuf(), master_outR, n_samps * sizeof(float));
}

void ProcMod2BandAGC::setup(const AGC_PARAMS _parms) {
    parms = _parms;
    /*
     typedef struct  {
         float drive;
         float release;
         float gate_thresh;
         bool use_coupling;
         float coupling;
         float window_size;
         float window_release;
         float ratio;
         float idle_gain;
         float attack;
     } COMPRESSOR_PARAMS;
     */
    comp_parms_lo = {
        .drive = parms.drive,
        .release = parms.release_bass,
        .gate_thresh = parms.gate_thresh,
        .use_coupling = true,
        .coupling = parms.bass_coupling,
        .window_size = parms.window_size,
        .window_release = parms.window_release,
        .ratio = parms.ratio,
        .idle_gain = parms.idle_gain,
        .attack = parms.attack_bass,
        .post_gain = parms.post_gain
    };
        
    comp_parms_hi = {
        .drive = parms.drive,
        .release = parms.release_master,
        .gate_thresh = parms.gate_thresh,
        .use_coupling = false,
        .coupling = 0.0,
        .window_size = parms.window_size,
        .window_release = parms.window_release,
        .ratio = parms.ratio,
        .idle_gain = parms.idle_gain,
        .attack = parms.attack_master,
        .post_gain = parms.post_gain
    };
    
    if(NULL != comp_lo) {
        delete comp_lo;
    }
    if(NULL != comp_hi) {
        delete comp_hi;
    }
    
    comp_lo = new Compressor(CompressorType::AGC, this->get_f_samp(), this->get_n_samps(), comp_parms_lo);
    comp_hi = new Compressor(CompressorType::AGC, this->get_f_samp(), this->get_n_samps(), comp_parms_hi);
    comp_lo->setup(comp_parms_lo);
    comp_hi->setup(comp_parms_hi);

}

void ProcMod2BandAGC::read(float* _gain_reduction_lo, float* _gain_reduction_hi, bool* _gate_open_lo, bool* _gate_open_hi) {
    comp_lo->read(_gain_reduction_lo, _gate_open_lo);
    comp_hi->read(_gain_reduction_hi, _gate_open_hi);
    
}

}
