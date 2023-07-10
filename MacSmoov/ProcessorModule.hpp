//
//  ProcessorModule.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 7/9/23.
//

#ifndef ProcessorModule_hpp
#define ProcessorModule_hpp

#include <stdio.h>
#include <cstdint>

namespace fmsmoov {

class ProcessorModule {
public:
    ProcessorModule(uint32_t _f_samp, uint32_t _n_channels);
    virtual ~ProcessorModule();
    /* A "frame" is a stereo pair of float samples. */
    virtual void process(float* inL, float* inR, float* outL, float* outR, uint32_t nframes) = 0;
    uint32_t get_f_samp();
    uint32_t get_n_channels();
private:
    uint32_t f_samp;
    uint8_t n_channels;
};

}

#endif /* ProcessorModule_hpp */
