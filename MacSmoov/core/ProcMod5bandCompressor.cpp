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

ProcMod5bandCompressor::ProcMod5bandCompressor(const string& _name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps) : ProcessorModule (_name, _f_samp, _n_channels, _n_samps) {
    
    //inbufs 0 and 1 will be stereo passthrough
    //inbufs 2-11 will be the L/R from the crossover bands 1-5
    
    this->set_out_buf(0, new AudioBuf(AudioBufType::REAL, "OUT_L", _n_samps));
    this->set_out_buf(1, new AudioBuf(AudioBufType::REAL, "OUT_R", _n_samps));
    
    linlogL = new LogLinConverter(LogLinConversionType::LIN_TO_LOG);
    linlogR = new LogLinConverter(LogLinConversionType::LIN_TO_LOG);
    
    comp_b1 = new Compressor(f_samp, n_samps, "5B COMP B1");
    comp_b2 = new Compressor(f_samp, n_samps, "5B COMP B2");
    comp_b3 = new Compressor(f_samp, n_samps, "5B COMP B3");
    comp_b4 = new Compressor(f_samp, n_samps, "5B COMP B4");
    comp_b5 = new Compressor(f_samp, n_samps, "5B COMP B5");
    
    gc_rawb1 = new float[n_samps]();
    gc_rawb2 = new float[n_samps]();
    gc_rawb3 = new float[n_samps]();
    gc_rawb4 = new float[n_samps]();
    gc_rawb5 = new float[n_samps]();
    
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
    
    lim_b1 = new Compressor(f_samp, n_samps, "5B LIM B1");
    lim_b2 = new Compressor(f_samp, n_samps, "5B LIM B2");
    lim_b3 = new Compressor(f_samp, n_samps, "5B LIM B3");
    lim_b4 = new Compressor(f_samp, n_samps, "5B LIM B4");
    lim_b5 = new Compressor(f_samp, n_samps, "5B LIM B5");
    
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
    
    delete[] gc_rawb1;
    delete[] gc_rawb2;
    delete[] gc_rawb3;
    delete[] gc_rawb4;
    delete[] gc_rawb5;
    
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
    
    if(params.band1_compressor_enabled) {
        comp_b1->compute_gc(inb1L, inb1R, gc_rawb1);
    } else {
        memset(gc_rawb1, 0, n_samps * sizeof(float));
    }
    
    if(params.band2_compressor_enabled) {
        comp_b2->compute_gc(inb2L, inb2R, gc_rawb2);
    } else {
        memset(gc_rawb2, 0, n_samps * sizeof(float));
    }
    
    if(params.band3_compressor_enabled) {
        comp_b3->compute_gc(inb3L, inb3R, gc_rawb3);
    } else {
        memset(gc_rawb3, 0, n_samps * sizeof(float));
    }
    
    if(params.band4_compressor_enabled) {
        comp_b4->compute_gc(inb4L, inb4R, gc_rawb4);
    } else {
        memset(gc_rawb4, 0, n_samps * sizeof(float));
    }
    
    if(params.band5_compressor_enabled) {
        comp_b5->compute_gc(inb5L, inb5R, gc_rawb5);
    } else {
        memset(gc_rawb5, 0, n_samps * sizeof(float));
    }
    
    /*
     *  Got the raw gain computation values because they can be coupled together in
     *  both directions.  Now go back and apply the coupling and do the compression.
     */
    
    /* BAND 1 */
    if(params.band1_mute) {
        memset(procb1L, 0, n_samps * sizeof(float));
        memset(procb1R, 0, n_samps * sizeof(float));
    }
    else if(params.band1_compressor_enabled) {
        /* Coupling is B2->B1 */
        comp_b1->process(inb1L, inb1R, procb1L, procb1R, n_samps, gc_rawb2, comp_b1_gain_reduction_buf);
    }
    else {
        memcpy(procb1L, inb1L, n_samps * sizeof(float));
        memcpy(procb1R, inb1R, n_samps * sizeof(float));
    }
    
    /* BAND 2 */
    if(params.band2_mute) {
        memset(procb2L, 0, n_samps * sizeof(float));
        memset(procb2R, 0, n_samps * sizeof(float));
    }
    else if(params.band2_compressor_enabled) {
        /* Coupling is B3->B2 */
        comp_b2->process(inb2L, inb2R, procb2L, procb2R, n_samps, gc_rawb3, comp_b2_gain_reduction_buf);
    }
    else {
        memcpy(procb2L, inb2L, n_samps * sizeof(float));
        memcpy(procb2R, inb2R, n_samps * sizeof(float));
    }
    
    /* BAND 3 */
    if(params.band3_mute) {
        memset(procb3L, 0, n_samps * sizeof(float));
        memset(procb3R, 0, n_samps * sizeof(float));
    }
    else if(params.band3_compressor_enabled) {
        /* Coupling is B2->B3 */
        comp_b3->process(inb3L, inb3R, procb3L, procb3R, n_samps, gc_rawb2, comp_b3_gain_reduction_buf);
    }
    else {
        memcpy(procb3L, inb3L, n_samps * sizeof(float));
        memcpy(procb3R, inb3R, n_samps * sizeof(float));
    }
    
    /* BAND 4 */
    if(params.band4_mute) {
        memset(procb4L, 0, n_samps * sizeof(float));
        memset(procb4R, 0, n_samps * sizeof(float));
    }
    else if(params.band4_compressor_enabled) {
        /* Coupling is B3->B4 */
        comp_b4->process(inb4L, inb4R, procb4L, procb4R, n_samps, gc_rawb3, comp_b4_gain_reduction_buf);
    }
    else {
        memcpy(procb4L, inb4L, n_samps * sizeof(float));
        memcpy(procb4R, inb4R, n_samps * sizeof(float));
    }
    
    /* BAND 5 */
    if(params.band5_mute) {
        memset(procb5L, 0, n_samps * sizeof(float));
        memset(procb5R, 0, n_samps * sizeof(float));
    }
    else if(params.band5_compressor_enabled) {
        /* Coupling is B4->B5 */
        comp_b5->process(inb5L, inb5R, procb5L, procb5R, n_samps, gc_rawb4, comp_b5_gain_reduction_buf);
    }
    else {
        memcpy(procb5L, inb5L, n_samps * sizeof(float));
        memcpy(procb5R, inb5R, n_samps * sizeof(float));
    }
    
    
    /* LIMITERS */
    
    /* BAND 1 */
    if(params.limiters_enabled && params.band1_limiter_enabled) {
        lim_b1->compute_gc(procb1L, procb1R);
        lim_b1->process(procb1L, procb1R, limb1L, limb1R, n_samps, NULL, lim_b1_gain_reduction_buf);
    }
    else {
        memcpy(limb1L, procb1L, n_samps * sizeof(float));
        memcpy(limb1R, procb1R, n_samps * sizeof(float));
    }
    
    /* BAND 2 */
    if(params.limiters_enabled && params.band2_limiter_enabled) {
        lim_b2->compute_gc(procb2L, procb2R);
        lim_b2->process(procb2L, procb2R, limb2L, limb2R, n_samps, NULL, lim_b2_gain_reduction_buf);
    }
    else {
        memcpy(limb2L, procb2L, n_samps * sizeof(float));
        memcpy(limb2R, procb2R, n_samps * sizeof(float));
    }
    
    /* BAND 3 */
    if(params.limiters_enabled && params.band3_limiter_enabled) {
        lim_b3->compute_gc(procb3L, procb3R);
        lim_b3->process(procb3L, procb3R, limb3L, limb3R, n_samps, NULL, lim_b3_gain_reduction_buf);
    }
    else {
        memcpy(limb3L, procb3L, n_samps * sizeof(float));
        memcpy(limb3R, procb3R, n_samps * sizeof(float));
    }
    
    /* BAND 4 */
    if(params.limiters_enabled && params.band4_limiter_enabled) {
        lim_b4->compute_gc(procb4L, procb4R);
        lim_b4->process(procb4L, procb4R, limb4L, limb4R, n_samps, NULL, lim_b4_gain_reduction_buf);
    }
    else {
        memcpy(limb4L, procb4L, n_samps * sizeof(float));
        memcpy(limb4R, procb4R, n_samps * sizeof(float));
    }
    
    /* BAND 5 */
    if(params.limiters_enabled && params.band5_limiter_enabled) {
        lim_b5->compute_gc(procb5L, procb5R);
        lim_b5->process(procb5L, procb5R, limb5L, limb5R, n_samps, NULL, lim_b5_gain_reduction_buf);
    }
    else {
        memcpy(limb5L, procb5L, n_samps * sizeof(float));
        memcpy(limb5R, procb5R, n_samps * sizeof(float));
    }
    
    for(uint32_t i = 0; i < n_samps; i++) {
        master_outL[i] = powf(10, params.master_post_gain/20.0f) * (limb1L[i] + limb2L[i] + limb3L[i] + limb4L[i] + limb5L[i]);
        master_outR[i] = powf(10, params.master_post_gain/20.0f) * (limb1R[i] + limb2R[i] + limb3R[i] + limb4R[i] + limb5R[i]);
    }
}

void ProcMod5bandCompressor::configure(const MULTIBAND_PARAMS _params) {
    params = _params;
    
    this->set_bypass(!_params.enabled);
    
    comp_b1->config(params.comp_params[0], true);
    comp_b2->config(params.comp_params[1], true);
    comp_b3->config(params.comp_params[2], true);
    comp_b4->config(params.comp_params[3], true);
    comp_b5->config(params.comp_params[4], true);
    
    //params.lim_params[0].thresh = params.comp_params[0].thresh + params.band1_comp_lim_offset;
    //params.lim_params[1].thresh = params.comp_params[1].thresh + params.band1_comp_lim_offset;
    //params.lim_params[2].thresh = params.comp_params[2].thresh + params.band1_comp_lim_offset;
    //params.lim_params[3].thresh = params.comp_params[3].thresh + params.band1_comp_lim_offset;
    //params.lim_params[4].thresh = params.comp_params[4].thresh + params.band1_comp_lim_offset;
    
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
