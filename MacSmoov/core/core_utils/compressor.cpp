#include "compressor.h"#include <stdexcept>#include <iostream>#include <math.h>using namespace std;namespace fmsmoov {Compressor::Compressor(CompressorType _type, uint32_t _samprate, uint32_t _n_samps, const COMPRESSOR_PARAMS& _params): type(_type), samprate(_samprate), n_samps(_n_samps), params(_params){    samp_converter_todb = new LogLinConverter(LogLinConversionType::LIN_TO_LOG);    indbL = new float[n_samps];    indbR = new float[n_samps];    indb = 0.0;    sc = 0.0;    gc = -20.0;    gs = -20.0;    gsPrev = -20.0;    recalculate();}Compressor::Compressor(const Compressor& rhs)  {    samprate = rhs.samprate;    n_samps = rhs.n_samps;    params = rhs.params;        samp_converter_todb = new LogLinConverter(LogLinConversionType::LIN_TO_LOG);        gsPrev = 0.0;    recalculate();}void Compressor::recalculate() {    alphaA = expf(-logf(9.0)/(samprate * params.tAtt));    alphaR = expf(-logf(9.0)/(samprate * params.tRel));    M = -(params.T - (params.T/params.R)); //dB}Compressor::~Compressor(){}void Compressor::setup(const COMPRESSOR_PARAMS& _params) {    params = _params;    recalculate();}void Compressor::process(float* inL, float* inR, float* outL, float* outR, uint32_t n_samps, float* gain_reduction_output, float* gain_coupling_input){    samp_converter_todb->process(inL, indbL, n_samps);    if(CompressorType::STEREO == type || CompressorType::AGC == type) {        samp_converter_todb->process(inR, indbR, n_samps);    }        for(uint32_t i = 0; i < n_samps; i++) {        if(CompressorType::STEREO == type || CompressorType::AGC == type) {            if(indbL[i] >= indbR[i]) {                indb = indbL[i];            }            else {                indb = indbR[i];            }        }        else {            indb = indbL[i];        }                if(CompressorType::AGC != type) {            if(indb < params.T){                sc = indb;            }            else {                sc = params.T + ((indb - params.T) / params.R);            }        }                if(CompressorType::AGC == type) {            if(indb < params.Tgate) {                gc = gsPrev;                gate_open = false;            }            else {                gc = params.T - indb;                gate_open = true;                if(NULL != gain_coupling_input) {                    gc = gc + ((gain_coupling_input[i] - gc) * params.gain_coupling_factor);                }            }                        if(gc > 0.0) {                gc = 0.0;            }        }        else {            gc = sc - indb;        }                if(gc <= gsPrev) {            gs = alphaA * gsPrev + (1.0f - alphaA) * gc;        }        else {            gs = alphaR * gsPrev + (1.0f - alphaR) * gc;        }                gsPrev = gs;                gain_reduction = gs;                if(NULL != gain_reduction_output) {            gain_reduction_output[i] = gs;        }                outL[i] = inL[i] * powf(10.0, (gs + params.G + M)/20.0);                if(CompressorType::STEREO == type || CompressorType::AGC == type) {            outR[i] = inR[i] * powf(10.0, (gs + params.G)/20.0);        }            }}void Compressor::read(float* _gain_reduction, bool* _gate_open) {    *_gain_reduction = gain_reduction;    *_gate_open = gate_open;}}