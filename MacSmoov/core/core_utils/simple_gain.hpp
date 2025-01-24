//
//  simple_gain.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 1/23/25.
//

#ifndef simple_gain_hpp
#define simple_gain_hpp

#include <stdio.h>
#include <cstdint>
#include <cmath>

#include "LogLinConverter.hpp"

namespace fmsmoov {

class SimpleGain {
public:
    SimpleGain(float _init_gain, uint32_t _n_samp);
    virtual ~SimpleGain();
    virtual void process(float* inL, float* inR, float* outL, float* outR);
    virtual void set_gain(float new_gain);
private:
    float gain;
    LogLinConverter* dbToLinL, *dbToLinR;
    LogLinConverter* linToDbL, *linToDbR;
    float* tmpL, *tmpR;
    uint32_t n_samp;
};

} //NAMESPACE FM_SMOOV

#endif /* simple_gain_hpp */
