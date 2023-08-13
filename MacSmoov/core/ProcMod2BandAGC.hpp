//
//  ProcMod2BandAGC.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 8/12/23.
//

#ifndef ProcMod2BandAGC_hpp
#define ProcMod2BandAGC_hpp

#include <stdio.h>
#include "ProcessorModule.hpp"
#include "filter_lr4.h"

namespace fmsmoov {

class ProcMod2BandAGC : public ProcessorModule {
public:
    ProcMod2BandAGC(const string& name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps);
    virtual ~ProcMod2BandAGC();
    /* n samps is total interleaved stereo samples
     TODO: Figure this shit out to make it universal. */
    virtual void process();
private:
    AudioBuf* buf_lin_in;
    AudioBuf* buf_db_in;
    AudioBuf* buf_out;
    FilterLR4* filt_lo_L, *filt_lo_R, *filt_hi_L, *filt_hi_R;
    float* buf_in_L, *buf_in_R, *testbuf;
    float* buf_lo_filtL, *buf_lo_filtR, *buf_hi_filtL, *buf_hi_filtR;
};

}

#endif /* ProcMod2BandCrossover150Hz_hpp */
