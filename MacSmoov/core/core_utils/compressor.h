#ifndef __COMPRESSOR_H__
#define __COMPRESSOR_H__

#include <unistd.h>
#include <stdint.h>

#include "LogLinConverter.hpp"

namespace fmsmoov {

enum class CompressorType {
    STEREO,
    MONO,
    AGC
};

typedef struct  {
    float T;
    float R;
    float G;
    float tAtt;
    float tRel;
    float gain_coupling_factor;
    float Tgate;
    float gate_hold_time;
} COMPRESSOR_PARAMS;

class Compressor
{
public:
    Compressor(CompressorType _type, uint32_t samprate, uint32_t _n_samps, const COMPRESSOR_PARAMS& _params);
    Compressor(const Compressor& rhs);
	virtual ~Compressor();
	void process(float* inL, float* inR, float* outL, float* outR, uint32_t n_samps, float* gainReductionOutput = NULL, float* gainCouplingInput = NULL);
    void setup(const COMPRESSOR_PARAMS& _params);
    void read(float* _gain_reduction, bool* _gate_open);

private:
    void recalculate();
    CompressorType type;
    uint32_t samprate, n_samps;
    COMPRESSOR_PARAMS params;
    float M;
    LogLinConverter* samp_converter_todb;
    float* indbL, *indbR;
    float alphaA, alphaR;
    float gsPrev;
    float indb, sc, gc, gs;
    float gain_reduction;
    bool gate_open;
};

}
#endif //#ifndef __COMPRESSOR__
