//
//  ProcModLookaheadLimiter.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 9/20/24.
//

#ifndef ProcModLookaheadLimiter_hpp
#define ProcModLookaheadLimiter_hpp

#include <stdio.h>
#include "ProcessorModule.hpp"
#include "lookahead_limiter.h"

namespace fmsmoov {

class ProcModLookaheadLimiter : public ProcessorModule {
public:
    ProcModLookaheadLimiter(const string& _name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps);
    virtual ~ProcModLookaheadLimiter();
    virtual bool init_impl(CoreConfig* cfg, ProcessorModule* prev_mod, ChannelMap* _channel_map);
    virtual void configure(const COMPRESSOR_PARAMS& _params);
    //TODO: Figure this shit out to make it universal. */
    virtual void process();
private:
    LookaheadLimiter* limiter;
    COMPRESSOR_PARAMS comp_params;
};

}

#endif /* ProcModLookaheadLimiter_hpp */
