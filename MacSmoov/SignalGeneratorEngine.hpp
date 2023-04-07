//
//  SignalGeneratorEngine.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 4/1/23.
//

#ifndef SignalGeneratorEngine_hpp
#define SignalGeneratorEngine_hpp

#include <stdio.h>
#include <cstdint>
#include <random>
#include "CommonTypes.h"

namespace fmsmoov {

class SignalGeneratorEngine {
public:
    SignalGeneratorEngine(uint32_t _f_samp, uint32_t _n_channels);
    virtual ~SignalGeneratorEngine();
    void configure(SIG_GEN_TYPE _type, uint32_t _frequency);
    void get_next_buffers(float* out_buf, uint32_t n_frames);
    void adjust_volume(float _volume);
private:
    SignalGeneratorEngine();
    float f_samp;
    uint32_t n_channels;
    SIG_GEN_TYPE type;
    float frequency;
    float volume;
    uint32_t cur_samp;
    float period_in_samples;
    float sawtooth_jump_size;
    float triangle_jump_size;
    float cur_sample_value;
    bool decrement;
};

}

#endif /* SignalGeneratorEngine_hpp */
