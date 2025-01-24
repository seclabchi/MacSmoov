#include "allpass_filter_lr4.h"
#include <cstring>
#include <iostream>

namespace tonekids {
namespace dsp {

#define LR4_FILTER_ORDER 2

AllpassFilterLR4::AllpassFilterLR4(double _fs, double _fc, uint32_t _n_samp) {
    fs = _fs;
    fc = _fc;
    n_samp = _n_samp;

    tmp_bufL = new float[n_samp];
    tmp_bufH = new float[n_samp];
    tmp_bufoutL = new float[n_samp];
    tmp_bufoutH = new float[n_samp];

    bwL = new Butterworth();
    bwH = new Butterworth();
    bqv1L_gain = 0.0;
    bqv2L_gain = 0.0;
    bqv1H_gain = 0.0;
    bqv2H_gain = 0.0;

    bwL->loPass(fs, fc, fc, LR4_FILTER_ORDER, bqv1L, bqv1L_gain);
    bwL->loPass(fs, fc, fc, LR4_FILTER_ORDER, bqv2L, bqv2L_gain);
    
    bwH->hiPass(fs, fc, fc, LR4_FILTER_ORDER, bqv1H, bqv1H_gain);
    bwH->hiPass(fs, fc, fc, LR4_FILTER_ORDER, bqv2H, bqv2H_gain);
    
}

bool AllpassFilterLR4::retune(double _fc) {
    fc = _fc;
    
    bwL->loPass(fs, fc, fc, LR4_FILTER_ORDER, bqv1L, bqv1L_gain);
    bwL->loPass(fs, fc, fc, LR4_FILTER_ORDER, bqv2L, bqv2L_gain);
    
    bwH->hiPass(fs, fc, fc, LR4_FILTER_ORDER, bqv1H, bqv1H_gain);
    bwH->hiPass(fs, fc, fc, LR4_FILTER_ORDER, bqv2H, bqv2H_gain);
    
    return true;
}

void AllpassFilterLR4::process(float* in, float* out) {
    bqv1L[0].process(in, tmp_bufL, n_samp);
    bqv2L[0].process(tmp_bufL, tmp_bufoutL, n_samp);
    bqv1H[0].process(in, tmp_bufH, n_samp);
    bqv2H[0].process(tmp_bufH, tmp_bufoutH, n_samp);
    
    for(uint32_t i = 0; i < n_samp; i++) {
        out[i] = tmp_bufoutL[i] + tmp_bufoutH[i];
    }
    
}

AllpassFilterLR4::~AllpassFilterLR4() {
    delete bwL;
    delete bwH;
    delete[] tmp_bufL;
    delete[] tmp_bufH;
    delete[] tmp_bufoutL;
    delete[] tmp_bufoutH;
}

} /* dsp */
} /* tonekids */
