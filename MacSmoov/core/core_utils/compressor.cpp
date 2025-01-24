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
    gs = -20.0f;
    gsPrev = -20.0f;
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
    memset(indbL, 0, n_samps*sizeof(float));
    indbR = new float[n_samps]();
    memset(indbR, 0, n_samps*sizeof(float));
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
    gs = -20.0;
    gsPrev = -20.0;
    alphaA = rhs.alphaA;
    alphaR_norm = rhs.alphaR_norm;
    alphaR_window = rhs.alphaR_window;
    M = rhs.M;
}

void Compressor::recalculate() {
    alphaA = expf(-logf(9.0f)/(samprate * params.attack));
    alphaR_norm = expf(-logf(9.0f)/(samprate * params.release));
    alphaR_window = expf(-logf(9.0f)/(samprate * window_release));
    M = -(params.target - (params.target/params.ratio)); //dB
}

Compressor::~Compressor()
{
    delete samp_converter_todb;
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

void Compressor::process(float* inL, float* inR, float* outL, float* outR, uint32_t n_samps, float* gain_reduction_output, float* gain_coupling_input, bool* force_gain, float* force_gain_target)
{
    gain_reduction = 0.0;
    gated_counter = 0.0;
    
    samp_converter_todb->process(inL, indbL, n_samps);
    samp_converter_todb->process(inR, indbR, n_samps);
    
    for(uint32_t i = 0; i < n_samps; i++) {
        if(indbL[i] >= indbR[i]) {
            indb = indbL[i];
        }
        else {
            indb = indbR[i];
        }
        
        if(indb < params.target) {
            sc = indb;
        }
        else {
            sc = params.target + ((indb - params.target) / params.ratio);
        }

        /* TODO: Implement windowing and coupling */
        
        gc = sc - indb;

        if(indb < params.thresh){
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
        
        if(NULL != gain_reduction_output) {
            gain_reduction_output[i] = gs;
        }
        
        outL[i] = inL[i] * powf(10.0f, gs/20.0f);
        outR[i] = inR[i] * powf(10.0f, gs/20.0f);
    }
    
    avg_gain_reduction = gain_reduction / (float)n_samps;
    
    is_gated = ((gated_counter/(float)n_samps) >= 0.5);
    
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
