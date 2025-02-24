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

    comp = new Compressor(samp_rate, n_samp, "LOOKAHEAD_LIMITER");
    gc_buf = new float[n_samp]();
    gs_buf = new float[n_samp]();

    input_delayed_L = new float[n_samp]();
    input_delayed_R = new float[n_samp]();
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

void LookaheadLimiter::configure(const COMPRESSOR_PARAMS& _params) {
    params = _params;
    comp->config(params);
    delay_L->config((uint32_t)(params.attack * 1000.0f));
    delay_R->config((uint32_t)(params.attack * 1000.0f));
}

void LookaheadLimiter::process(float* inL, float* inR, float* outL, float* outR) {

    comp->compute_gc(inL, inR, gc_buf);
    comp->compute_gs(inL, inR, gc_buf, gs_buf);

    delay_L->process(inL, input_delayed_L, n_samp);
    delay_R->process(inR, input_delayed_R, n_samp);

    comp->apply_gs(input_delayed_L, input_delayed_R, outL, outR, gs_buf);
}

} //namespace FMSMOOV
