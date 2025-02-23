//
//  ProcModInputHPF.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 2/9/25.
//

#ifndef ProcModInputHPF_hpp
#define ProcModInputHPF_hpp

#include <stdio.h>
#include "ProcessorModule.hpp"
#include "core_common.h"
#include "filter_lr4.h"
#include "AudioBuf.hpp"

namespace fmsmoov {

class ProcModInputHPF : public ProcessorModule {
public:
    ProcModInputHPF(const string& name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps);
    virtual ~ProcModInputHPF();
    virtual bool init_impl(CoreConfig* cfg, ProcessorModule* prev_mod, ChannelMap* _channel_map);
    /* n samps is total interleaved stereo samples
     TODO: Figure this shit out to make it universal. */
    virtual void process();
private:
    tonekids::dsp::FilterLR4* filt_L, *filt_R;
    float* buf_filtL, *buf_filtR;
};

}

#endif /* ProcModInputHPF_hpp */
