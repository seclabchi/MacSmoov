//
//  ProcessorCore.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 4/15/23.
//

#ifndef ProcessorCore_hpp
#define ProcessorCore_hpp

#include <stdio.h>
#include <cstdint>

namespace fmsmoov {

class ProcessorCore {
public:
    ProcessorCore(uint32_t _f_samp, uint32_t _n_channels);
    void process(float* in_samp, float* out_samp, uint32_t n_frames);
private:
    ProcessorCore();
    float f_samp;
    uint32_t n_channels;
};

}
#endif /* ProcessorCore_hpp */
