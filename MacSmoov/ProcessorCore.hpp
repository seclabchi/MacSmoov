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

#include "ProcModLevelMeter.hpp"
#include "LogLinConverter.hpp"

namespace fmsmoov {

class ProcessorCore {
public:
    ProcessorCore(uint32_t _f_samp, uint32_t _n_channels, uint32_t _buf_size);
    void process(float* in_samp, float* out_samp, uint32_t n_samps);
private:
    ProcessorCore();
    float f_samp;
    uint32_t n_channels;
    uint32_t buf_size;
    
    ProcModLevelMeter* proc_mod_level_main_in;
    LogLinConverter* m_loglin;
    LogLinConverter* m_linlog;
    
};

}
#endif /* ProcessorCore_hpp */
