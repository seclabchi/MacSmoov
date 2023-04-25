//
//  ProcessorCore.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 4/15/23.
//

#include "ProcessorCore.hpp"
#include <string>

void ProcessorCore::process(float* in_samp, float* out_samp, uint32_t n_frames) {
    //TODO: Assumes stereo interleaved float samples, and that n_frames assumes a pair of float samples.
    memcpy(out_samp, in_samp, n_frames * 2 * sizeof(float));
}
