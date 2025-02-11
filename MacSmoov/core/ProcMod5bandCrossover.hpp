//
//  ProcMod5bandCrossover.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 8/28/23.
//

#ifndef ProcMod5bandCrossover_hpp
#define ProcMod5bandCrossover_hpp

#include <stdio.h>
#include "ProcessorModule.hpp"
#include "simple_gain.hpp"
#include "crossover_5_band.h"

namespace fmsmoov {

class ProcMod5bandCrossover : public ProcessorModule {
public:
    ProcMod5bandCrossover(const string& name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps);
    virtual ~ProcMod5bandCrossover();
    virtual bool init_impl(CoreConfig* cfg, ProcessorModule* prev_mod, ChannelMap* _channel_map);
    /* n samps is total interleaved stereo samples
     TODO: Figure this shit out to make it universal. */
    virtual void process();
    virtual void configure(float _drive);
    void band_enable(bool b1, bool b2, bool b3, bool b4, bool b5);
private:
    SimpleGain* mb_drive;
    AudioBuf* post_drive_bufL, *post_drive_bufR;
    float* inL, *inR, *outL, *outR;
    float** b1out, **b2out, **b3out, **b4out, **b5out;
    Crossover_5_band* crossover;
};

}

#endif /* ProcMod5bandCrossover_hpp */
