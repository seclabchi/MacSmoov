//
//  ProcMod5bandCompressor.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 8/26/23.
//

#ifndef ProcMod5bandCompressor_hpp
#define ProcMod5bandCompressor_hpp

#include <stdio.h>
#include "ProcessorModule.hpp"
#include "crossover_5_band.h"
#include "compressor.h"
#include "LogLinConverter.hpp"
#include "filter_lr4.h"

namespace fmsmoov {



class ProcMod5bandCompressor : public ProcessorModule {
public:
    ProcMod5bandCompressor(const string& name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps);
    virtual ~ProcMod5bandCompressor();
    /* n samps is total interleaved stereo samples
     TODO: Figure this shit out to make it universal. */
    virtual void process();
    void setup(const MULTIBAND_PARAMS _parms);
    void read(float** _bands_gr);
private:
    float* master_outL, *master_outR;
    float* procb1L, *procb1R, *procb2L, *procb2R, *procb3L, *procb3R, *procb4L, *procb4R, *procb5L, *procb5R;
    
    float* inb1L, *inb1R, *inb2L, *inb2R, *inb3L, *inb3R, *inb4L, *inb4R, *inb5L, *inb5R;
    
    Compressor* comp_b1;
    Compressor* comp_b2;
    Compressor* comp_b3;
    Compressor* comp_b4;
    Compressor* comp_b5;
    
    float* comp_b1_gain_reduction_buf;
    float* comp_b2_gain_reduction_buf;
    float* comp_b3_gain_reduction_buf;
    float* comp_b4_gain_reduction_buf;
    float* comp_b5_gain_reduction_buf;
    
    MULTIBAND_PARAMS params;
    COMPRESSOR_PARAMS comp_parms_b1;
    COMPRESSOR_PARAMS comp_parms_b2;
    COMPRESSOR_PARAMS comp_parms_b3;
    COMPRESSOR_PARAMS comp_parms_b4;
    COMPRESSOR_PARAMS comp_parms_b5;
    
    bool first_setup_complete;
    
    LogLinConverter* linlogL;
    LogLinConverter* linlogR;
};

}

#endif /* ProcMod5bandCompressor */
