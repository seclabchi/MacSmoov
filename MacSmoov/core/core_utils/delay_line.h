#ifndef __DELAY_LINE_H__
#define __DELAY_LINE_H__

#include <unistd.h>
#include <stdint.h>

#include "core_common.h"

#include <cstdint>

namespace fmsmoov {


class DelayLine
{
public:
    DelayLine(uint32_t _max_delay, uint32_t _samp_rate, uint32_t _n_samps);
    DelayLine(const DelayLine& rhs);
	virtual ~DelayLine();
    void recalculate();
	void process(float* in, float* out, uint32_t n_samps);
    void config(uint32_t _delay_ms);
private:
    void read_samp_buf(float* out, uint32_t n);
    void write_samp_buf(float* in, uint32_t n);

private:
    uint32_t max_delay;
    uint32_t samp_rate;
    uint32_t n_samps;
    uint32_t delay_ms;
    uint32_t d_buf_len;  //length in samples of the delay buffer
    uint32_t d_nsamp; //number of samples of delay
    float* d_buf;
    float* d_buf_tail;
    float* rPtr, *wPtr;
    uint64_t r_ptr_offset, w_ptr_offset;
    uint64_t rDTE, wDTE;  //distance to end of delay buffer
};

}
#endif //#ifndef __COMPRESSOR__
