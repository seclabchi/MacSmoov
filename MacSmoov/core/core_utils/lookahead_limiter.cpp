//
//  lookahead_limiter.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 2/22/25.
//

#include "lookahead_limiter.h"
#include <cstring>

namespace fmsmoov {

LookaheadLimiter::LookaheadLimiter(uint32_t _samp_rate, uint32_t _n_samp)
    : samp_rate(_samp_rate), n_samp(_n_samp) {
    delay_L = new DelayLine(100, samp_rate, n_samp);
    delay_R = new DelayLine(100, samp_rate, n_samp);

    comp = new Compressor(samp_rate, n_samp);
    gc_buf = new float[n_samp]();
    gs_buf = new float[n_samp]();

    input_delayed_L = new float[n_samp]();
    input_delayed_R = new float[n_samp]();

    thresh = -6.0f;
    attack = 20.0f;
    hold = 10.0f;
    release = 40.0f;
    ratio = 100.0f;

    comp_params = COMPRESSOR_PARAMS {
        .thresh = thresh,
        .gate_thresh = -100.0f,
        .attack = attack,
        .release = release,
        .ratio = ratio
    };

    comp->config(comp_params);
}

LookaheadLimiter::~LookaheadLimiter() {
    delete delay_L;
    delete delay_R;
    delete comp;
    delete[] gc_buf;
    delete[] gs_buf;
    delete[] input_delayed_L;
    delete[] input_delayed_R;
}

void LookaheadLimiter::configure(float _thresh, float _attack, float _hold, float _release, float _ratio) {
    thresh = _thresh;
    attack = _attack;
    hold = _hold;
    release = _release;
    ratio = _ratio;
    comp_params = COMPRESSOR_PARAMS {
        .thresh = thresh,
        .gate_thresh = -100.0f,
        .attack = attack,
        .release = release,
        .ratio = ratio
    };
    comp->config(comp_params);
    delay_L->config(comp_params.attack);
    delay_R->config(comp_params.attack);
}

void LookaheadLimiter::process(float* inL, float* inR, float* outL, float* outR) {

    //memcpy(outL, inL, n_samp*sizeof(float));
    //memcpy(outR, inR, n_samp*sizeof(float));
    //return;

    comp->compute_gc(inL, inR, gc_buf);
    comp->compute_gs(inL, inR, gc_buf, gs_buf);

    delay_L->process(inL, input_delayed_L, n_samp);
    delay_R->process(inR, input_delayed_R, n_samp);

    //memcpy(outL, gc_buf, n_samp * sizeof(float));
    //memcpy(outR, gs_buf, n_samp * sizeof(float));
    //memcpy(outL, input_delayed_L, n_samp*sizeof(float));
    //memcpy(outR, input_delayed_R, n_samp*sizeof(float));
    //return;

    comp->apply_gs(input_delayed_L, input_delayed_R, outL, outR, gs_buf);
}

} //namespace FMSMOOV
