//
//  ProcModStereoEnhance.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 8/14/23.
//

#ifndef ProcModStereoEnhance_hpp
#define ProcModStereoEnhance_hpp

#include <stdio.h>
#include <vector>
#include "ProcessorModule.hpp"
#include "vca.h"
#include "butterworth.h"
#include "comparator.hpp"

#define SQRT_2 1.41421356237f

namespace fmsmoov {

class ProcModStereoEnhance : public ProcessorModule {
public:
    ProcModStereoEnhance(const string& name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps);
    virtual ~ProcModStereoEnhance();
    virtual bool init_impl(CoreConfig* cfg, ProcessorModule* prev_mod, ChannelMap* _channel_map);
    /* n samps is total interleaved stereo samples
     TODO: Figure this shit out to make it universal. */
    virtual void configure();
    virtual void process();
    void read(float* _lrdiff);
private:
    void subtract_buffers(float* buf1, float* buf2, float* out);
    void add_buffers(float* buf1, float* buf2, float* out);
    float* inL, *inR, *outL, *outR;
    float avg_lr_diff;
    
    VCA* vca60;
    tonekids::dsp::Butterworth* bpf62;
    Comparator* comparator65;
    
    std::vector<tonekids::dsp::Biquad> bpf62_bq;
    //double bpf62_bq_gain;
    
    float* line17;
    float* vca_ctrl_39;
    float* comp_enh_l_minus_r;
    float* bpf62_out;
    float* comparator65_out;
};

}

#endif /* ProcModStereoEnhance.hpp */
