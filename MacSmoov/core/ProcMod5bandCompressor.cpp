//
//  ProcMod5bandCompressor.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 8/26/23.
//

#include "ProcMod5bandCompressor.hpp"
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
    
    comp_b1 = new Compressor(f_samp, n_samps);
    comp_b2 = new Compressor(f_samp, n_samps);
    comp_b3 = new Compressor(f_samp, n_samps);
    comp_b4 = new Compressor(f_samp, n_samps);
    comp_b5 = new Compressor(f_samp, n_samps);
    
    comp_b1_gain_reduction_buf = new float[n_samps]();
    comp_b2_gain_reduction_buf = new float[n_samps]();
    comp_b3_gain_reduction_buf = new float[n_samps]();
    comp_b4_gain_reduction_buf = new float[n_samps]();
    comp_b5_gain_reduction_buf = new float[n_samps]();
    
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
    
    lim_b1 = new Compressor(f_samp, n_samps);
    lim_b2 = new Compressor(f_samp, n_samps);
    lim_b3 = new Compressor(f_samp, n_samps);
    lim_b4 = new Compressor(f_samp, n_samps);
    lim_b5 = new Compressor(f_samp, n_samps);
    
    limb1L = new float[n_samps]();
    limb1R = new float[n_samps]();
    limb2L = new float[n_samps]();
    limb2R = new float[n_samps]();
    limb3L = new float[n_samps]();
    limb3R = new float[n_samps]();
    limb4L = new float[n_samps]();
    limb4R = new float[n_samps]();
    limb5L = new float[n_samps]();
    limb5R = new float[n_samps]();
    
    lim_b1_gain_reduction_buf = new float[n_samps]();
    lim_b2_gain_reduction_buf = new float[n_samps]();
    lim_b3_gain_reduction_buf = new float[n_samps]();
    lim_b4_gain_reduction_buf = new float[n_samps]();
    lim_b5_gain_reduction_buf = new float[n_samps]();
    
    first_setup_complete = false;
    
}

ProcMod5bandCompressor::~ProcMod5bandCompressor() {
    
    delete[] comp_b1_gain_reduction_buf;
    delete[] comp_b2_gain_reduction_buf;
    delete[] comp_b3_gain_reduction_buf;
    delete[] comp_b4_gain_reduction_buf;
    delete[] comp_b5_gain_reduction_buf;
    
    delete[] lim_b1_gain_reduction_buf;
    delete[] lim_b2_gain_reduction_buf;
    delete[] lim_b3_gain_reduction_buf;
    delete[] lim_b4_gain_reduction_buf;
    delete[] lim_b5_gain_reduction_buf;
    
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
    
    delete[] limb1L;
    delete[] limb1R;
    delete[] limb2L;
    delete[] limb2R;
    delete[] limb3L;
    delete[] limb3R;
    delete[] limb4L;
    delete[] limb4R;
    delete[] limb5L;
    delete[] limb5R;
    
}

bool ProcMod5bandCompressor::init_impl(CoreConfig* cfg, ProcessorModule* prev_mod, ChannelMap* _channel_map) {
    if(nullptr != cfg) {
        //TODO config
    }
    if((nullptr != _channel_map) && (nullptr != prev_mod)) {
        for(CHANNEL_MAP_ELEMENT e : _channel_map->the_map)
        {
            this->set_in_buf(e.this_chan, prev_mod->get_out_buf(e.in_chan));
        }
    }

    MULTIBAND_PARAMS mb_params;
    cfg->get_mb_params(mb_params);
    this->configure(mb_params);
    
    return true;
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
    
    /* BAND 1 */
    if(band_mute[0]) {
        memset(procb1L, 0, n_samps * sizeof(float));
        memset(procb1R, 0, n_samps * sizeof(float));
    }
    else if(compressor_band_enabled[0]) {
        comp_b1->process(inb1L, inb1R, procb1L, procb1R, n_samps, comp_b1_gain_reduction_buf, NULL);
    }
    else {
        memcpy(procb1L, inb1L, n_samps * sizeof(float));
        memcpy(procb1R, inb1R, n_samps * sizeof(float));
    }
    
    /* BAND 2 */
    if(band_mute[1]) {
        memset(procb2L, 0, n_samps * sizeof(float));
        memset(procb2R, 0, n_samps * sizeof(float));
    }
    else if(compressor_band_enabled[1]) {
        comp_b2->process(inb2L, inb2R, procb2L, procb2R, n_samps, comp_b2_gain_reduction_buf, NULL);
    }
    else {
        memcpy(procb2L, inb2L, n_samps * sizeof(float));
        memcpy(procb2R, inb2R, n_samps * sizeof(float));
    }
    
    /* BAND 3 */
    if(band_mute[2]) {
        memset(procb3L, 0, n_samps * sizeof(float));
        memset(procb3R, 0, n_samps * sizeof(float));
    }
    else if(compressor_band_enabled[2]) {
        comp_b3->process(inb3L, inb3R, procb3L, procb3R, n_samps, comp_b3_gain_reduction_buf, NULL);
    }
    else {
        memcpy(procb3L, inb3L, n_samps * sizeof(float));
        memcpy(procb3R, inb3R, n_samps * sizeof(float));
    }
    
    /* BAND 4 */
    if(band_mute[3]) {
        memset(procb4L, 0, n_samps * sizeof(float));
        memset(procb4R, 0, n_samps * sizeof(float));
    }
    else if(compressor_band_enabled[3]) {
        comp_b4->process(inb4L, inb4R, procb4L, procb4R, n_samps, comp_b4_gain_reduction_buf, comp_b3_gain_reduction_buf);
    }
    else {
        memcpy(procb4L, inb4L, n_samps * sizeof(float));
        memcpy(procb4R, inb4R, n_samps * sizeof(float));
    }
    
    /* BAND 5 */
    if(band_mute[4]) {
        memset(procb5L, 0, n_samps * sizeof(float));
        memset(procb5R, 0, n_samps * sizeof(float));
    }
    else if(compressor_band_enabled[4]) {
        comp_b5->process(inb5L, inb5R, procb5L, procb5R, n_samps, comp_b5_gain_reduction_buf, comp_b3_gain_reduction_buf);
    }
    else {
        memcpy(procb5L, inb5L, n_samps * sizeof(float));
        memcpy(procb5R, inb5R, n_samps * sizeof(float));
    }
    
    
    /* LIMITERS */
    
    /* BAND 1 */
    if(limiter_band_enabled[0]) {
        lim_b1->process(procb1L, procb1R, limb1L, limb1R, n_samps, lim_b1_gain_reduction_buf, NULL);
    }
    else {
        memcpy(limb1L, procb1L, n_samps * sizeof(float));
        memcpy(limb1R, procb1R, n_samps * sizeof(float));
    }
    
    /* BAND 2 */
    if(limiter_band_enabled[1]) {
        lim_b2->process(procb2L, procb2R, limb2L, limb2R, n_samps, lim_b2_gain_reduction_buf, NULL);
    }
    else {
        memcpy(limb2L, procb2L, n_samps * sizeof(float));
        memcpy(limb2R, procb2R, n_samps * sizeof(float));
    }
    
    /* BAND 3 */
    if(limiter_band_enabled[2]) {
        lim_b3->process(procb3L, procb3R, limb3L, limb3R, n_samps, lim_b3_gain_reduction_buf, NULL);
    }
    else {
        memcpy(limb3L, procb3L, n_samps * sizeof(float));
        memcpy(limb3R, procb3R, n_samps * sizeof(float));
    }
    
    /* BAND 4 */
    if(limiter_band_enabled[3]) {
        lim_b4->process(procb4L, procb4R, limb4L, limb4R, n_samps, lim_b4_gain_reduction_buf, NULL);
    }
    else {
        memcpy(limb4L, procb4L, n_samps * sizeof(float));
        memcpy(limb4R, procb4R, n_samps * sizeof(float));
    }
    
    /* BAND 5 */
    if(limiter_band_enabled[4]) {
        lim_b5->process(procb5L, procb5R, limb5L, limb5R, n_samps, lim_b5_gain_reduction_buf, NULL);
    }
    else {
        memcpy(limb5L, procb5L, n_samps * sizeof(float));
        memcpy(limb5R, procb5R, n_samps * sizeof(float));
    }
    
    for(uint32_t i = 0; i < n_samps; i++) {
        master_outL[i] = limb1L[i] + limb2L[i] + limb3L[i] + limb4L[i] + limb5L[i];
        master_outR[i] = limb1R[i] + limb2R[i] + limb3R[i] + limb4R[i] + limb5R[i];
    }
}

void ProcMod5bandCompressor::configure(const MULTIBAND_PARAMS _params) {
    params = _params;
    
    comp_b1->config(params.comp_params[0]);
    comp_b2->config(params.comp_params[1]);
    comp_b3->config(params.comp_params[2]);
    comp_b4->config(params.comp_params[3]);
    comp_b5->config(params.comp_params[4]);
    lim_b1->config(params.lim_params[0]);
    lim_b2->config(params.lim_params[1]);
    lim_b3->config(params.lim_params[2]);
    lim_b4->config(params.lim_params[3]);
    lim_b5->config(params.lim_params[4]);
    
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

void ProcMod5bandCompressor::read(float** _bands_gr, float** _bands_lim, bool** _bands_gate_open) {
    comp_b1->read(_bands_gr[0], _bands_gate_open[0]);
    comp_b2->read(_bands_gr[1], _bands_gate_open[1]);
    comp_b3->read(_bands_gr[2], _bands_gate_open[2]);
    comp_b4->read(_bands_gr[3], _bands_gate_open[3]);
    comp_b5->read(_bands_gr[4], _bands_gate_open[4]);
    lim_b1->read(_bands_lim[0], NULL);
    lim_b2->read(_bands_lim[1], NULL);
    lim_b3->read(_bands_lim[2], NULL);
    lim_b4->read(_bands_lim[3], NULL);
    lim_b5->read(_bands_lim[4], NULL);
}

}
