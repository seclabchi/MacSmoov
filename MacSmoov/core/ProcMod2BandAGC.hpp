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
#include "core_common.h"
#include "compressor.h"
#include "LogLinConverter.hpp"
#include "filter_lr4.h"

namespace fmsmoov {

class ProcMod2BandAGC : public ProcessorModule {
public:
    ProcMod2BandAGC(const string& name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps);
    virtual ~ProcMod2BandAGC();
    virtual bool init_impl(CoreConfig* cfg, ProcessorModule* prev_mod, ChannelMap* _channel_map);
    /* n samps is total interleaved stereo samples
     TODO: Figure this shit out to make it universal. */
    virtual void process();
    void setup(const AGC_PARAMS& _parms);
    void read(float* _gain_reduction_lo, float* _gain_reduction_hi, bool* _gate_open_lo, bool* _gate_open_hi);
private:
    FilterLR4* filt_lo_L, *filt_lo_R, *filt_hi_L, *filt_hi_R;
    float* buf_lo_filtL, *buf_lo_filtR, *buf_hi_filtL, *buf_hi_filtR;
    float* master_outL, *master_outR;
    
    float* agc_out_loL, *agc_out_hiL, *agc_out_loR, *agc_out_hiR;
    Compressor* comp_lo;
    Compressor* comp_hi;
    float* comp_hi_gain_reduction_buf;
    
    AGC_PARAMS parms;
    COMPRESSOR_PARAMS comp_parms_lo;
    COMPRESSOR_PARAMS comp_parms_hi;
    
    LogLinConverter* linlogL;
    LogLinConverter* linlogR;
};

}

#endif /* ProcMod2BandAGC_hpp */
