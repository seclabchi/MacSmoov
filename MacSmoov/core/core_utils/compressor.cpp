#include "compressor.h"
#include <stdexcept>
#include <iostream>
#include <math.h>

using namespace std;

namespace fmsmoov {

Compressor::Compressor(CompressorType _type, uint32_t _samprate, uint32_t _n_samps)
: type(_type), samprate(_samprate), n_samps(_n_samps)
{
    samp_converter_todb = new LogLinConverter(LogLinConversionType::LIN_TO_LOG);
    indbL = new float[n_samps]();
    memset(indbL, 0, n_samps*sizeof(float));
    indbR = new float[n_samps]();
    memset(indbR, 0, n_samps*sizeof(float));
    gate_open = false;
    avg_gain_reduction = 0.0;
    gain_reduction = 0.0;
    indb = 0.0;
    sc = 0.0;
    gc = 0.0;
    gs = -20.0;
    gsPrev = -20.0;
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
    gate_open = false;
    avg_gain_reduction = 0.0;
    gain_reduction = 0.0;
    indb = 0.0;
    sc = 0.0;
    gc = 0.0;
    gs = -20.0;
    gsPrev = -20.0;
    recalculate();
}

void Compressor::recalculate() {
    alphaA = expf(-logf(9.0f)/(samprate * params.attack));
    alphaR_norm = expf(-logf(9.0f)/(samprate * params.release));
    alphaR_window = expf(-logf(9.0f)/(samprate * params.window_release));
    M = -(params.drive - (params.drive/params.ratio)); //dB
}

Compressor::~Compressor()
{

}

void Compressor::setup(const COMPRESSOR_PARAMS& _params) {
    params = _params;
    recalculate();
}

void Compressor::process(float* inL, float* inR, float* outL, float* outR, uint32_t n_samps, float* gain_reduction_output, float* gain_coupling_input)
{
    gain_reduction = 0.0;
    gate_open_calc = 0.0;
    samp_converter_todb->process(inL, indbL, n_samps);
    if(CompressorType::STEREO == type) {
        samp_converter_todb->process(inR, indbR, n_samps);
    }
    
    for(uint32_t i = 0; i < n_samps; i++) {
        if(CompressorType::STEREO == type) {
            if(indbL[i] >= indbR[i]) {
                indb = indbL[i];
            }
            else {
                indb = indbR[i];
            }
        }
        else {
            indb = indbL[i];
        }
        
        if(indb < params.drive) {
            sc = indb;
        }
        else {
            sc = params.drive + ((indb - params.drive) / params.ratio);
        }
        
        /* TODO: This section probably ain't right. Figure the coupling out even when the gate is closed. */
        
        
        if(indb < params.gate_thresh) {
            if(params.use_coupling && NULL != gain_coupling_input) {
                gc = ((1.0f - params.coupling) * (gsPrev)) + (params.coupling * gain_coupling_input[i]);
            }
            else {
                gc = gsPrev;
            }
            gate_open = false;
            gate_open_calc++;
        }
        else {
            if(params.use_coupling && NULL != gain_coupling_input) {
                gc = ((1.0f - params.coupling) * (sc - indb)) + (params.coupling * gain_coupling_input[i]);
            }
            else {
                gc = sc - indb;
            }
            gate_open = true;
        }
        
        if(gc <= gsPrev) {
            gs = alphaA * gsPrev + (1.0f - alphaA) * gc;
        }
        else {
            /* TODO:  THIS IS PROBABLY WRONG, figure out the window */
            if(gc >= params.window_size) {
                gs = alphaR_window * gsPrev + (1.0f - alphaR_window) * gc;
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
        
        outL[i] = inL[i] * powf(10.0f, (gs + params.post_gain)/20.0f);
        
        if(CompressorType::STEREO == type) {
            outR[i] = inR[i] * powf(10.0f, (gs + params.post_gain)/20.0f);
        }
        
    }
    
    avg_gain_reduction = gain_reduction / (float)n_samps;
    
}

void Compressor::read(float* _gain_reduction, bool* _gate_open) {
    if(NULL != _gain_reduction) {
        *_gain_reduction = avg_gain_reduction;
    }
    
    if(NULL != _gate_open) {
        if(gate_open_calc/(float)n_samps >= 0.5) {
            *_gate_open = false;
        }
        else {
            *_gate_open = true;
        }
    }
}

}
