//
//  ProcMod5bandCompressor.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 8/26/23.
//

#include "ProcMod5bandCompressor.hpp"
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

ProcMod5bandCompressor::ProcMod5bandCompressor(const string& _name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps) : ProcessorModule (_name, _f_samp, _n_channels, _n_samps) {
    
    //inbufs 0 and 1 will be stereo passthrough
    //inbufs 2-11 will be the L/R from the crossover bands 1-5
    
    this->set_out_buf(0, new AudioBuf(AudioBufType::REAL, "OUT_L", _n_samps));
    this->set_out_buf(1, new AudioBuf(AudioBufType::REAL, "OUT_R", _n_samps));
    
    linlogL = new LogLinConverter(LogLinConversionType::LIN_TO_LOG);
    linlogR = new LogLinConverter(LogLinConversionType::LIN_TO_LOG);
    
    COMPRESSOR_PARAMS dummy;
    
    comp_b1 = new Compressor(CompressorType::STEREO, f_samp, n_samps, dummy);
    comp_b2 = new Compressor(CompressorType::STEREO, f_samp, n_samps, dummy);
    comp_b3 = new Compressor(CompressorType::STEREO, f_samp, n_samps, dummy);
    comp_b4 = new Compressor(CompressorType::STEREO, f_samp, n_samps, dummy);
    comp_b5 = new Compressor(CompressorType::STEREO, f_samp, n_samps, dummy);
    
    procb1L = new float[n_samps]();
    procb1R = new float[n_samps]();
    procb2L = new float[n_samps]();
    procb2R = new float[n_samps]();
    procb3L = new float[n_samps]();
    procb3R = new float[n_samps]();
    procb4L = new float[n_samps]();
    procb4R = new float[n_samps]();
    procb5L = new float[n_samps]();
    procb5R = new float[n_samps]();
    
    first_setup_complete = false;
    
}

ProcMod5bandCompressor::~ProcMod5bandCompressor() {
    
    delete[] comp_b1_gain_reduction_buf;
    delete[] comp_b2_gain_reduction_buf;
    delete[] comp_b3_gain_reduction_buf;
    delete[] comp_b4_gain_reduction_buf;
    delete[] comp_b5_gain_reduction_buf;
    
    delete[] procb1L;
    delete[] procb1R;
    delete[] procb2L;
    delete[] procb2R;
    delete[] procb3L;
    delete[] procb3R;
    delete[] procb4L;
    delete[] procb4R;
    delete[] procb5L;
    delete[] procb5R;
}

void ProcMod5bandCompressor::process() {
    uint32_t n_samps = this->get_n_samps();
    master_outL = this->get_out_buf(0)->getbuf();
    master_outR = this->get_out_buf(1)->getbuf();
    
    if(this->get_bypass()) {
        memcpy(this->get_out_buf(0)->getbuf(), this->get_in_buf(0)->getbuf(), n_samps * sizeof(float));
        memcpy(this->get_out_buf(1)->getbuf(), this->get_in_buf(1)->getbuf(), n_samps * sizeof(float));
        return;
    }
    
    comp_b1->process(inb1L, inb1R, procb1L, procb1R, n_samps, NULL, comp_b1_gain_reduction_buf);
    comp_b2->process(inb2L, inb2R, procb2L, procb2R, n_samps, NULL, comp_b2_gain_reduction_buf);
    comp_b3->process(inb3L, inb3R, procb3L, procb3R, n_samps, NULL, comp_b3_gain_reduction_buf);
    comp_b4->process(inb4L, inb4R, procb4L, procb4R, n_samps, NULL, comp_b4_gain_reduction_buf);
    comp_b5->process(inb5L, inb5R, procb5L, procb5R, n_samps, NULL, comp_b5_gain_reduction_buf);
    
    for(uint32_t i = 0; i < n_samps; i++) {
        master_outL[i] = procb1L[i] + procb2L[i] + procb3L[i] + procb4L[i] + procb5L[i];
        master_outR[i] = procb1R[i] + procb2R[i] + procb3R[i] + procb4R[i] + procb5R[i];
    }
}

void ProcMod5bandCompressor::setup(const MULTIBAND_PARAMS _params) {
    params = _params;
    comp_b1->setup(params.band_params[0]);
    comp_b2->setup(params.band_params[1]);
    comp_b3->setup(params.band_params[2]);
    comp_b4->setup(params.band_params[3]);
    comp_b5->setup(params.band_params[4]);
    
    if(!first_setup_complete) {
        inb1L = this->get_in_buf(2)->getbuf();
        inb1R = this->get_in_buf(3)->getbuf();
        inb2L = this->get_in_buf(4)->getbuf();
        inb2R = this->get_in_buf(5)->getbuf();
        inb3L = this->get_in_buf(6)->getbuf();
        inb3R = this->get_in_buf(7)->getbuf();
        inb4L = this->get_in_buf(8)->getbuf();
        inb4R = this->get_in_buf(9)->getbuf();
        inb5L = this->get_in_buf(10)->getbuf();
        inb5R = this->get_in_buf(11)->getbuf();
        first_setup_complete = true;
    }
}

void ProcMod5bandCompressor::read(float** _bands_gr) {
    comp_b1->read(_bands_gr[0], NULL);
    comp_b2->read(_bands_gr[1], NULL);
    comp_b3->read(_bands_gr[2], NULL);
    comp_b4->read(_bands_gr[3], NULL);
    comp_b5->read(_bands_gr[4], NULL);
}

}
