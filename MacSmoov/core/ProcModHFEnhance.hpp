//
//  ProcModHFEnhance.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 9/16/23.
//

#ifndef ProcModHFEnhance_hpp
#define ProcModHFEnhance_hpp

#include <stdio.h>
#include "ProcessorModule.hpp"
#include "core_common.h"
#include "core_utils/filter_3rdorder.h"
#include "core_utils/filter_el.h"
#include "compressor.h"
#include "LogLinConverter.hpp"

namespace fmsmoov {

class ProcModHFEnhance : public ProcessorModule {
public:
    ProcModHFEnhance(const string& name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps);
    virtual ~ProcModHFEnhance();
    /* n samps is total interleaved stereo samples
     TODO: Figure this shit out to make it universal. */
    virtual void process();
    //void setup(const AGC_PARAMS _parms);
    //void read(float* _gain_reduction_lo, float* _gain_reduction_hi, bool* _gate_open_lo, bool* _gate_open_hi);
private:
    Filter3rdOrder* filt_110L, *filt_110R;
    FilterEL* filt_120L, *filt_120R;
    float* buf_130L, *buf_130R;
    float* buf_140L, *buf_140R;
    float* buf_150L, *buf_150R, *buf_160L, *buf_160R;
    float* master_outL, *master_outR;
    
    LogLinConverter* linlogL;
    LogLinConverter* linlogR;
};

}

#endif /* ProcModHFEnhance_hpp */
