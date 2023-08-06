//
//  LPFSinglePole.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 7/11/23.
//

#ifndef LPFSinglePole_hpp
#define LPFSinglePole_hpp

#include <stdio.h>
#include <stdint.h>

/* Assumes interleaved stereo samples */

class LPFSinglePole {
public:
    LPFSinglePole(uint32_t _f_samp, float _tau);
    virtual ~LPFSinglePole();
    void process(float* in, float* out, uint32_t n_samp);
private:
    uint32_t f_samp;
    float t_samp;
    float rc;
    float alpha;
    float yprevL, yprevR;
};

#endif /* LPFSinglePole_hpp */
