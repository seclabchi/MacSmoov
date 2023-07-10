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

namespace fmsmoov {

class ProcModLevelMeter : public ProcessorModule {
public:
    ProcModLevelMeter(uint32_t _f_samp, uint8_t _n_channels);
    virtual ~ProcModLevelMeter();
    /* A "frame" is a stereo pair of float samples. */
    virtual void process(float* inL, float* inR, float* outL, float* outR, uint32_t nframes);
    /* For now, just compute RMS levels per invocation of the process command */
    void get_levels(float* L, float* R);
private:
    float rms_l;
    float rms_r;
};

}

#endif /* ProcModLevelMeter_hpp */
