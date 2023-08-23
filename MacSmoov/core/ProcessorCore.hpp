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
#include "ProcModStereoEnhance.hpp"
#include "LogLinConverter.hpp"

namespace fmsmoov {

using namespace std;

class ProcessorCore {
public:
    ProcessorCore(uint32_t _f_samp, uint32_t _n_channels, uint32_t _n_samp);
    void process(float* in_L, float* in_R, float* out_L, float* out_R, uint32_t n_samp);
    void get_main_in_levels(float* lrms, float* rrms, float* lpeak, float* rpeak);
    void set_main_in_gain_db(float loggain_l, float loggain_r);
    void get2bandAGCGainReduction(float* gainReduct2blo, float* gainReduct2bhi, bool* gateOpenLo, bool* gateOpenHi);
private:
    ProcessorCore();
    float f_samp;
    uint32_t n_channels;
    uint32_t n_samp;  //total interleaved L+R samples
    
    ProcModGain* proc_mod_gain_main_in;
    ProcModLevelMeter* proc_mod_level_main_in;
    ProcMod2BandAGC* proc_mod_2band_agc;
    ProcModStereoEnhance* proc_mod_stereo_enhance;
    LogLinConverter* m_loglin;
    LogLinConverter* m_linlog;
    
};

}
#endif /* ProcessorCore_hpp */
