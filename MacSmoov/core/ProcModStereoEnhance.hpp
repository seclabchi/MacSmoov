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

namespace fmsmoov {

class ProcModStereoEnhance : public ProcessorModule {
public:
    ProcModStereoEnhance(const string& name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps);
    virtual ~ProcModStereoEnhance();
    virtual bool init_impl(CoreConfig* cfg, ProcessorModule* prev_mod, ChannelMap* _channel_map);
    /* n samps is total interleaved stereo samples
     TODO: Figure this shit out to make it universal. */
    virtual void process();
private:
    void do_summer_add(float* in1, float* in2, float* out, uint32_t n_samps);
    void do_summer_subtract(float* in1, float* in2, float* out, uint32_t n_samps);
    void do_vca_23(float* in, float* control, float* out, uint32_t n_samps);
    void do_vca_60(float* in, float* control, float* out, uint32_t n_samps);
    float* inL, *inR, *outL, *outR;
    float* line39, *line57, *line61, *line17, *line19;
    VCA* vca59, *vca60, *vca23;
};

}

#endif /* ProcModStereoEnhance.hpp */
