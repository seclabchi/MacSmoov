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

class ProcessorCore {
    void process(float* in_samp, float* out_samp, uint32_t n_frames);
};

#endif /* ProcessorCore_hpp */
