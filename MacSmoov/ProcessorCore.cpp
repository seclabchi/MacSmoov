//
//  ProcessorCore.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 4/15/23.
//

#include "ProcessorCore.hpp"
#include <string>


using namespace fmsmoov;

ProcessorCore::ProcessorCore(uint32_t _f_samp, uint32_t _n_channels, uint32_t _buf_size) : f_samp(_f_samp), n_channels(_n_channels), buf_size(_buf_size) {
    proc_mod_level_main_in = new ProcModLevelMeter(f_samp, n_channels, buf_size);
}

void ProcessorCore::process(float* in_samp, float* out_samp, uint32_t n_samps) {
    //TODO: Assumes stereo interleaved float samples, and that n_bytes is the total size of the data buffer.
    //For now, just assume 2 channels, one float L and one float R.
    //memcpy(out_samp, in_samp, n_bytes * sizeof(float));
    proc_mod_level_main_in->process(in_samp, out_samp, n_samps);
}
