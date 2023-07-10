//
//  ProcessorCore.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 4/15/23.
//

#include "ProcessorCore.hpp"
#include <string>

using namespace fmsmoov;

ProcessorCore::ProcessorCore(uint32_t _f_samp, uint32_t _n_channels) : f_samp(_f_samp), n_channels(_n_channels) {
    proc_mod_level_main_in = new ProcModLevelMeter(f_samp, n_channels);
}

void ProcessorCore::process(float* in_samp, float* out_samp, uint32_t n_frames) {
    //TODO: Assumes stereo interleaved float samples, and that n_frames assumes a pair of float samples.
    memcpy(out_samp, in_samp, n_frames * sizeof(float));
}
