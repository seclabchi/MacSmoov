//
//  ProcModLevelMeter.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 7/9/23.
//

#ifndef ProcModLevelMeter_hpp
#define ProcModLevelMeter_hpp

#include <stdio.h>
#include "ProcessorModule.hpp"
#include "LPFSinglePole.hpp"

namespace fmsmoov {

class ProcModLevelMeter : public ProcessorModule {
public:
    ProcModLevelMeter(const string& _name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps);
    virtual ~ProcModLevelMeter();
    virtual bool init_impl(CoreConfig* cfg, ProcessorModule* prev_mod, ChannelMap* _channel_map);
    /* n samps is total interleaved stereo samples
     TODO: Figure this shit out to make it universal. */
    virtual void process();
    void get_levels_db(float* rmsL, float* rmsR, float* peakL, float* peakR);
private:
    LPFSinglePole* lpf10ms;
    float* prefiltered;
    float rms_l;
    float rms_r;
    float peak_l;
    float peak_r;
    float tmp_samp_mag;
    float out_rms_l, out_rms_r, out_peak_l, out_peak_r;
};

}

#endif /* ProcModLevelMeter_hpp */
