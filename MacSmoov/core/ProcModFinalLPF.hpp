//
//  ProcModFinalLPF.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 2/9/25.
//

#ifndef ProcModFinalLPF_hpp
#define ProcModFinalLPF_hpp

#include <stdio.h>
#include "ProcessorModule.hpp"
#include "core_common.h"
#include "filter_lr4.h"

namespace fmsmoov {

class ProcModFinalLPF : public ProcessorModule {
public:
    ProcModFinalLPF(const string& name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps);
    virtual ~ProcModFinalLPF();
    virtual bool init_impl(CoreConfig* cfg, ProcessorModule* prev_mod, ChannelMap* _channel_map);
    /* n samps is total interleaved stereo samples
     TODO: Figure this shit out to make it universal. */
    virtual void process();
private:
    tonekids::dsp::FilterLR4* filt_L, *filt_R;
    float* buf_filtL, *buf_filtR;
};

}

#endif /* ProcModFinalLPF_hpp */
