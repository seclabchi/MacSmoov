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
    /* _buf_size is number of samples per channel */
    ProcessorModule(uint32_t _f_samp, uint32_t _n_channels, uint32_t _buf_size);
    virtual ~ProcessorModule();
    /* A "frame" is a stereo pair of float samples. */
    virtual void process(float* in, float* out, uint32_t n_samps) = 0;
    uint32_t get_f_samp();
    uint32_t get_n_channels();
    uint32_t get_buf_size();
private:
    uint32_t f_samp;
    uint8_t n_channels;
    uint32_t buf_size;
};

}

#endif /* ProcessorModule_hpp */
