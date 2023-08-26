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
    float drive;
    float release;
    float gate_thresh;
    bool use_coupling;
    float coupling;
    float window_size;
    float window_release;
    float ratio;
    float idle_gain;
    float attack;
    float post_gain;
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
    float alphaA, alphaR_norm, alphaR_window;
    float gsPrev;
    float indb, sc, gc, gs;
    float gain_reduction;
    bool gate_open;
};

}
#endif //#ifndef __COMPRESSOR__
