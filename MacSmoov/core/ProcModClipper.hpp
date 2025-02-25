//
//  ProcModClipper.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 2/12/25.
//

#ifndef ProcModClipper_hpp
#define ProcModClipper_hpp

#include <stdio.h>
#include "ProcessorModule.hpp"
#include "core_common.h"
#include "filter_lr4.h"

namespace fmsmoov {

class ProcModClipper : public ProcessorModule {
public:
    ProcModClipper(const string& name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps);
    virtual ~ProcModClipper();
    virtual bool init_impl(CoreConfig* cfg, ProcessorModule* prev_mod, ChannelMap* _channel_map);
    /* n samps is total interleaved stereo samples
     TODO: Figure this shit out to make it universal. */
    virtual void process();
    virtual void configure(float _clip_level);
    void read(float* _actionL, float* _actionR);

private:
    float clip_level;
    float avg_clip_linear_L, avg_clip_linear_R;
    float avg_clip_db_L, avg_clip_db_R;;
};

}

#endif /* ProcModFinalLPF_hpp */
