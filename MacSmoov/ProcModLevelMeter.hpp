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
    ProcModLevelMeter(uint32_t _f_samp, uint8_t _n_channels, uint32_t _buf_size);
    virtual ~ProcModLevelMeter();
    /* n samps is total interleaved stereo samples
     TODO: Figure this shit out to make it universal. */
    virtual void process(float* in, float* out, uint32_t n_samps);
    /* For now, just compute RMS levels per invocation of the process command */
    void get_levels(float* L, float* R);
private:
    LPFSinglePole* lpf10ms;
    float* prefiltered;
    float rms_l;
    float rms_r;
    float peak_l;
    float peak_r;
    float tmp_samp_mag;
};

}

#endif /* ProcModLevelMeter_hpp */
