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
#include "ProcModGain.hpp"
#include "ProcMod2BandAGC.hpp"
#include "LogLinConverter.hpp"

namespace fmsmoov {

using namespace std;

class ProcessorCore {
public:
    ProcessorCore(uint32_t _f_samp, uint32_t _n_channels, uint32_t _buf_size);
    void process(float* in_samp, float* out_samp, uint32_t n_samps);
    void get_main_in_levels(float* lrms, float* rrms, float* lpeak, float* rpeak);
    void set_main_in_gain_db(float loggain_l, float loggain_r);
private:
    ProcessorCore();
    float f_samp;
    uint32_t n_channels;
    uint32_t buf_size;
    uint32_t m_buf_samps;  //number of samples sent to the process call, both channels interleaved.  # of floats.
    float* m_tmpbufA;
    float* m_tmpbufB;
    
    ProcModGain* proc_mod_gain_main_in;
    ProcModLevelMeter* proc_mod_level_main_in;
    ProcMod2BandAGC* proc_mod_2band_agc;
    LogLinConverter* m_loglin;
    LogLinConverter* m_linlog;
    
};

}
#endif /* ProcessorCore_hpp */
