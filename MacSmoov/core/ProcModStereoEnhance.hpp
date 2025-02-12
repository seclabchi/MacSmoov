//
//  ProcModStereoEnhance.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 8/14/23.
//

#ifndef ProcModStereoEnhance_hpp
#define ProcModStereoEnhance_hpp

#include <stdio.h>
#include "ProcessorModule.hpp"
#include "vca_proportional.h"

#define SQRT_2 1.41421356237f

namespace fmsmoov {

class ProcModStereoEnhance : public ProcessorModule {
public:
    ProcModStereoEnhance(const string& name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps);
    virtual ~ProcModStereoEnhance();
    virtual bool init_impl(CoreConfig* cfg, ProcessorModule* prev_mod, ChannelMap* _channel_map);
    /* n samps is total interleaved stereo samples
     TODO: Figure this shit out to make it universal. */
    virtual void configure(float _drive);
    virtual void process();
    void read(float* _lrdiff);
private:
    float* inL, *inR, *outL, *outR;
    float drive;
    float M, S;
    float avg_lr_diff;
};

}

#endif /* ProcModStereoEnhance.hpp */
