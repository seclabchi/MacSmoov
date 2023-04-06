//
//  SignalGenerator.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 4/1/23.
//

#ifndef SignalGenerator_hpp
#define SignalGenerator_hpp

#include <stdio.h>
#include <cstdint>
#include "CommonTypes.h"

namespace fmsmoov {

class SignalGenerator {
public:
    SignalGenerator(uint32_t _f_samp, uint32_t _n_channels);
    virtual ~SignalGenerator();
    void configure(SIG_GEN_TYPE _type, uint32_t _frequency);
    void get_next_buffers(float* out_buf, uint32_t n_frames);
    void adjust_volume(float _volume);
private:
    SignalGenerator();
    float f_samp;
    uint32_t n_channels;
    SIG_GEN_TYPE type;
    float frequency;
    float volume;
};

}

#endif /* SignalGenerator_hpp */
