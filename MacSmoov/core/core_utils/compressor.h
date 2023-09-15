#ifndef __COMPRESSOR_H__
#define __COMPRESSOR_H__

#include <unistd.h>
#include <stdint.h>

#include "core_common.h"
#include "LogLinConverter.hpp"

namespace fmsmoov {

enum class CompressorType {
    STEREO,
    MONO
};


class Compressor
{
public:
    Compressor(CompressorType _type, uint32_t samprate, uint32_t _n_samps);
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
    float alphaA, alphaR_norm, alphaR_window;
    float gsPrev;
    float indb, sc, gc, gs;
    float avg_gain_reduction, gain_reduction;
    bool gate_open;
    float gate_open_calc;
};

}
#endif //#ifndef __COMPRESSOR__
