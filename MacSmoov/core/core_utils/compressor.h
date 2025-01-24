#ifndef __COMPRESSOR_H__
#define __COMPRESSOR_H__

#include <unistd.h>
#include <stdint.h>

#include "core_common.h"
#include "LogLinConverter.hpp"

namespace fmsmoov {

class Compressor
{
public:
    Compressor(uint32_t samprate, uint32_t _n_samps);
    Compressor(const Compressor& rhs);
	virtual ~Compressor();
	void process(float* inL, float* inR, float* outL, float* outR, uint32_t n_samps, float* gainReductionOutput = NULL, float* gainCouplingInput = NULL, bool* force_gain = NULL, float* force_gain_target = NULL);
    void config(const COMPRESSOR_PARAMS& _params, const bool _use_coupling = false, const float coupling = 0.0, const bool _use_window = false,
                const float _window_size = 0.001, const float _window_release = 0.001);
    void read(float* _gain_reduction, bool* _gate_open);

private:
    void recalculate();
    COMPRESSOR_PARAMS params;
    uint32_t samprate, n_samps;
    bool use_coupling, use_window;
    float coupling, window_size, window_release;
    float M;
    LogLinConverter* samp_converter_todb;
    float* indbL, *indbR;
    float alphaA, alphaR_norm, alphaR_window;
    float gsPrev;
    float indb, sc, gc, gs;
    float avg_gain_reduction, gain_reduction;
    bool is_gated;
    float gated_counter;
};

}
#endif //#ifndef __COMPRESSOR__
