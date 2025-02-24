#ifndef __COMPRESSOR_H__
#define __COMPRESSOR_H__

#include <unistd.h>
#include <stdint.h>
#include <string>

#include "core_common.h"
#include "LogLinConverter.hpp"

namespace fmsmoov {


class Compressor
{
public:
    Compressor(uint32_t samprate, uint32_t _n_samps, const std::string& _name = "UNNAMED");
    Compressor(const Compressor& rhs);
	virtual ~Compressor();

    /* HOKEY: You must call compute_gc before process for each frame. */
	void process(float* inL, float* inR, float* outL, float* outR, uint32_t n_samps, float* gainCouplingInput = NULL,
                               float* gainReductionOutput = NULL);
    void compute_gc(float* inL, float* inR, float* _gc_out = NULL);
    void compute_gs(float* in, float* out, float* _gc_in, float* _gs_out);
    void apply_gs(float* inL, float* inR, float* outL, float* outR, float* _gs_in);
    void config(const COMPRESSOR_PARAMS& _params, const bool _use_coupling = false, const float coupling = 0.0, const bool _use_window = false,
                const float _window_size = 0.001f, const float _window_release = 0.001f);
    void read(float* _gain_reduction, bool* _gate_open);

private:
    std::string name;
    void recalculate();
    COMPRESSOR_PARAMS params;
    uint32_t samprate, n_samps;
    bool use_coupling, use_window;
    float coupling, window_size, window_release;
    COMPRESSOR_KNEE_TYPE knee_type;
    float knee_width;
    float M;
    LogLinConverter* samp_converter_todb;
    float* indbL, *indbR;

    float* gc_raw;
    float* gs_raw;

    float alphaA, alphaR_norm, alphaR_window;
    float gsPrev;
    float gs_prev;
    float indb, sc, gc, gs;
    float gc_prev;
    float avg_gain_reduction, gain_reduction;
    bool is_gated;
    float gated_counter;
    bool compute_gc_has_been_called;
    bool compute_gs_has_been_called;
};

}
#endif //#ifndef __COMPRESSOR__
