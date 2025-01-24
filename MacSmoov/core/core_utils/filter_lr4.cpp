#include "filter_lr4.h"
#include <cstring>
#include <iostream>

namespace tonekids {
namespace dsp {

#define LR4_FILTER_ORDER 2

FilterLR4::FilterLR4(double _fs, double _fc, tonekids::dsp::BUTTERWORTH_TYPE _type, uint32_t _n_samp) {
    fs = _fs;
    fc = _fc;
    type = _type;
    n_samp = _n_samp;

    tmp_buf = new float[n_samp];

    bw = new Butterworth();
    bqv1_gain = 0.0;
    bqv2_gain = 0.0;

    if(_type == tonekids::dsp::kLoPass) {
        bw->loPass(fs, fc, fc, LR4_FILTER_ORDER, bqv1, bqv1_gain);
        bw->loPass(fs, fc, fc, LR4_FILTER_ORDER, bqv2, bqv2_gain);
    }
    else if(_type == tonekids::dsp::kHiPass) {
        bw->hiPass(fs, fc, fc, LR4_FILTER_ORDER, bqv1, bqv1_gain);
        bw->hiPass(fs, fc, fc, LR4_FILTER_ORDER, bqv2, bqv2_gain);
    }
    else {
        std::cerr << "Invalid filter type for LR4" << std::endl;
    }
}

bool FilterLR4::retune(double _fc, tonekids::dsp::BUTTERWORTH_TYPE _type) {
    fc = _fc;
    type = _type;
    
    if(_type == tonekids::dsp::kLoPass) {
        bw->loPass(fs, fc, fc, LR4_FILTER_ORDER, bqv1, bqv1_gain);
        bw->loPass(fs, fc, fc, LR4_FILTER_ORDER, bqv2, bqv2_gain);
    }
    else if(_type == tonekids::dsp::kHiPass) {
        bw->hiPass(fs, fc, fc, LR4_FILTER_ORDER, bqv1, bqv1_gain);
        bw->hiPass(fs, fc, fc, LR4_FILTER_ORDER, bqv2, bqv2_gain);
    }
    else {
        std::cerr << "Invalid filter type for LR4" << std::endl;
        return false;
    }
    
    return true;
}

void FilterLR4::process(float* in, float* out) {
    bqv1[0].process(in, tmp_buf, n_samp);
    bqv2[0].process(tmp_buf, out, n_samp);
}

FilterLR4::~FilterLR4() {
    delete bw;
    delete[] tmp_buf;
}

} /* dsp */
} /* tonekids */
