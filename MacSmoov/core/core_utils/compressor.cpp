#include "compressor.h"#include <stdexcept>#include <iostream>#include <math.h>using namespace std;namespace fmsmoov {Compressor::Compressor(CompressorType _type, uint32_t _samprate, uint32_t _n_samps, const COMPRESSOR_PARAMS& _params): type(_type), samprate(_samprate), n_samps(_n_samps), params(_params){    samp_converter_todb = new LogLinConverter(LogLinConversionType::LIN_TO_LOG);    indbL = new float[n_samps];    indbR = new float[n_samps];    indb = 0.0;    sc = 0.0;    gc = -20.0;    gs = -20.0;    gsPrev = -20.0;    recalculate();}Compressor::Compressor(const Compressor& rhs)  {    samprate = rhs.samprate;    n_samps = rhs.n_samps;    params = rhs.params;        samp_converter_todb = new LogLinConverter(LogLinConversionType::LIN_TO_LOG);        gsPrev = 0.0;    recalculate();}void Compressor::recalculate() {    alphaA = expf(-logf(9.0)/(samprate * params.attack));    alphaR_norm = expf(-logf(9.0)/(samprate * params.release));    alphaR_window = expf(-logf(9.0)/(samprate * params.window_release));    M = -(params.drive - (params.drive/params.ratio)); //dB}Compressor::~Compressor(){}void Compressor::setup(const COMPRESSOR_PARAMS& _params) {    params = _params;    recalculate();}void Compressor::process(float* inL, float* inR, float* outL, float* outR, uint32_t n_samps, float* gain_reduction_output, float* gain_coupling_input){    samp_converter_todb->process(inL, indbL, n_samps);    if(CompressorType::STEREO == type || CompressorType::AGC == type) {        samp_converter_todb->process(inR, indbR, n_samps);    }        for(uint32_t i = 0; i < n_samps; i++) {        if(CompressorType::STEREO == type || CompressorType::AGC == type) {            if(indbL[i] >= indbR[i]) {                indb = indbL[i];            }            else {                indb = indbR[i];            }        }        else {            indb = indbL[i];        }                if(indb < params.drive) {            sc = indb;        }        else {            sc = params.drive + ((indb - params.drive) / params.ratio);        }                /* TODO: This section ain't right. Figure the coupling out even when the gate is closed. */                if(CompressorType::AGC == type) {            if(indb < params.gate_thresh) {                if(params.use_coupling) {                    gc = gsPrev + (params.coupling * (gain_coupling_input[i] - gc));                }                else {                    gc = gsPrev;                }                gate_open = false;            }            else {                if(params.use_coupling) {                    gc = (sc - indb) + (params.coupling * (gain_coupling_input[i] - gc));                }                else {                    gc = sc - indb;                }                gate_open = true;            }        }        else {            gc = sc - indb;        }                if(gc <= gsPrev) {            gs = alphaA * gsPrev + (1.0f - alphaA) * gc;        }        else {            /* TODO:  THIS IS WRONG */            if(gc >= params.window_size) {                gs = alphaR_window * gsPrev + (1.0f - alphaR_window) * gc;            }            else {                gs = alphaR_norm * gsPrev + (1.0f - alphaR_norm) * gc;            }        }                gsPrev = gs;                gain_reduction = gs;                if(NULL != gain_reduction_output) {            gain_reduction_output[i] = gs;        }                outL[i] = inL[i] * powf(10.0, (gs + params.post_gain)/20.0);                if(CompressorType::STEREO == type || CompressorType::AGC == type) {            outR[i] = inR[i] * powf(10.0, (gs + params.post_gain)/20.0);        }            }}void Compressor::read(float* _gain_reduction, bool* _gate_open) {    *_gain_reduction = gain_reduction;    *_gate_open = gate_open;}}