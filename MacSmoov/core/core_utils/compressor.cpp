#include "compressor.h"
#include <stdexcept>
#include <iostream>
#include <math.h>

using namespace std;

namespace fmsmoov {

Compressor::Compressor(uint32_t _samprate, uint32_t _n_samps)
: samprate(_samprate), n_samps(_n_samps)
{
    samp_converter_todb = new LogLinConverter(LogLinConversionType::LIN_TO_LOG);
    indbL = new float[n_samps]();
    memset(indbL, 0, n_samps*sizeof(float));
    indbR = new float[n_samps]();
    memset(indbR, 0, n_samps*sizeof(float));
    gc_raw = new float[n_samps]();
    memset(gc_raw, 0, n_samps*sizeof(float));
    use_coupling = false;
    coupling = 0.0;
    use_window = false;
    window_size = 0.001;
    window_release = 0.001;
    is_gated = true;
    avg_gain_reduction = 0.0f;
    gain_reduction = 0.0f;
    indb = 0.0f;
    sc = 0.0f;
    gc = 0.0f;
    gs = -10.0f;
    gsPrev = -10.0f;
    alphaA = 1.0f;
    alphaR_norm = 1.0f;
    alphaR_window = 1.0f;
    M = 1.0f;
}

Compressor::Compressor(const Compressor& rhs)  {
    samprate = rhs.samprate;
    n_samps = rhs.n_samps;
    params = rhs.params;
    indbL = new float[n_samps]();
    memcpy(indbL, rhs.indbL, n_samps*sizeof(float));
    indbR = new float[n_samps]();
    memcpy(indbR, rhs.indbR, n_samps*sizeof(float));
    gc_raw = new float[n_samps]();
    memcpy(gc_raw, rhs.gc_raw, n_samps*sizeof(float));
    samp_converter_todb = new LogLinConverter(LogLinConversionType::LIN_TO_LOG);
    use_coupling = rhs.use_coupling;
    coupling = rhs.coupling;
    use_window = rhs.use_window;
    window_size = rhs.window_size;
    window_release = rhs.window_release;
    gated_counter = 0.0;
    is_gated = rhs.is_gated;
    avg_gain_reduction = rhs.avg_gain_reduction;
    gain_reduction = rhs.gain_reduction;
    indb = 0.0;
    sc = 0.0;
    gc = 0.0;
    gs = -10.0f;
    gsPrev = -10.0f;
    alphaA = rhs.alphaA;
    alphaR_norm = rhs.alphaR_norm;
    alphaR_window = rhs.alphaR_window;
    M = rhs.M;
}

void Compressor::recalculate() {
    alphaA = expf(-logf(9.0f)/(samprate * params.attack));
    alphaR_norm = expf(-logf(9.0f)/(samprate * params.release));
    alphaR_window = expf(-logf(9.0f)/(samprate * window_release));
}

Compressor::~Compressor()
{
    delete samp_converter_todb;
    delete[] indbR;
    delete[] indbL;
    delete[] gc_raw;
}

void Compressor::config(const COMPRESSOR_PARAMS& _params, const bool _use_coupling, const float _coupling, const bool _use_window,
                        const float _window_size, const float _window_release) {
    params = _params;
    use_coupling = _use_coupling;
    coupling = _coupling;
    use_window = _use_window;
    window_size = _window_size;
    window_release = _window_release;
    recalculate();
}

void Compressor::compute_gc(float* inL, float* inR, float* _gc_raw) {
    samp_converter_todb->process(inL, indbL, n_samps);
    samp_converter_todb->process(inR, indbR, n_samps);
    
    for(uint32_t i = 0; i < n_samps; i++) {
        if(indbL[i] >= indbR[i]) {
            indb = indbL[i];
        }
        else {
            indb = indbR[i];
        }
        
        if(indb < params.thresh) {
            sc = indb;
        }
        else {
            sc = params.thresh + ((indb - params.thresh) / params.ratio);
        }
        
        gc_raw[i] = sc - indb;
        
        if(_gc_raw) {
            _gc_raw[i] = gc_raw[i];
        }
    }
    
    compute_gc_has_been_called = true;
}

/* HOKEY: You must call compute_gc before this function for each frame. */
void Compressor::process(float* inL, float* inR, float* outL, float* outR, uint32_t n_samps, float* gainCouplingInput,
                              float* gainReductionOutput) {
    
    if(!compute_gc_has_been_called) {
        throw std::runtime_error("gc_compute has not been called on this frame");
    }
    
    gain_reduction = 0.0;
    gated_counter = 0.0;
    
    samp_converter_todb->process(inL, indbL, n_samps);
    samp_converter_todb->process(inR, indbR, n_samps);
    
    for(uint32_t i = 0; i < n_samps; i++) {
        
        /* TODO: Implement windowing and coupling */
                
        if(!use_coupling) {
            gc = gc_raw[i];
        }
        else {
            if(NULL == gainCouplingInput) {
                throw std::runtime_error("gainCouplingInput is NULL");
            }
            gc = ((1-coupling) * gc_raw[i]) + (coupling * gainCouplingInput[i]);
        }
        
        if(indb < params.gate_thresh) {
            gs = gsPrev;
            gated_counter++;
        }
        else {
            if(gc <= gsPrev) {
                gs = alphaA * gsPrev + (1.0f - alphaA) * gc;
            }
            else {
                gs = alphaR_norm * gsPrev + (1.0f - alphaR_norm) * gc;
            }
        }
    
        gsPrev = gs;
        
        gain_reduction += gs;
        
        if(NULL != gainReductionOutput) {
            gainReductionOutput[i] = gs;
        }
        
        outL[i] = inL[i] * powf(10.0f, gs/20.0f);
        outR[i] = inR[i] * powf(10.0f, gs/20.0f);
    }
    
    avg_gain_reduction = gain_reduction / (float)n_samps;
    
    is_gated = ((gated_counter/(float)n_samps) >= 0.5);
    compute_gc_has_been_called = false;
}

void Compressor::read(float* _gain_reduction, bool* _gate_open) {
    if(NULL != _gain_reduction) {
        *_gain_reduction = avg_gain_reduction;
    }
    
    if(NULL != _gate_open) {
        *_gate_open = !is_gated;
    }
}

}
