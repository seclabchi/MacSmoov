//
//  lookahead_limiter.h
//  MacSmoov
//
//  Created by Paul Zaremba on 2/22/25.
//

#ifndef lookahead_limiter_h
#define lookahead_limiter_h

#include <stdio.h>
#include <cstdint>
#include <cmath>

#include "LogLinConverter.hpp"
#include "delay_line.h"
#include "compressor.h"

namespace fmsmoov {

class LookaheadLimiter {
public:
    LookaheadLimiter(uint32_t _samp_rate, uint32_t _n_samp);
    virtual ~LookaheadLimiter();
    virtual void configure(const COMPRESSOR_PARAMS& _params);
    virtual void process(float* inL, float* inR, float* outL, float* outR);
private:
    COMPRESSOR_PARAMS params;
    DelayLine* delay_L;
    DelayLine* delay_R;
    Compressor* comp;
    float* gc_buf;
    float* gs_buf;
    float* input_delayed_L;
    float* input_delayed_R;
    COMPRESSOR_PARAMS comp_params;

    LogLinConverter* dbToLinL, *dbToLinR;
    LogLinConverter* linToDbL, *linToDbR;
    uint32_t samp_rate;
    uint32_t n_samp;
};

} //NAMESPACE FM_SMOOV

#endif /* lookahead_limiter_h */
