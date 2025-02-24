#include "delay_line.h"
#include <stdexcept>
#include <iostream>
#include <math.h>

using namespace std;

namespace fmsmoov {

DelayLine::DelayLine(uint32_t _max_delay, uint32_t _samp_rate, uint32_t _n_samps)
: max_delay(_max_delay), samp_rate(_samp_rate), n_samps(_n_samps)
{
    /*
     * size the delay buffer for the max possible delay
     * plus the block size of the samples that are
     * processed.  Buffer size must be 2x max delay!
    */
    d_buf_len = (_samp_rate * ((float)max_delay * (2.0f/1000.0f)));
    d_buf = (float*) calloc(sizeof(float), d_buf_len);
    d_buf_tail = d_buf + d_buf_len - 1;
    d_nsamp = 0;
    delay_ms = 0;

    rPtr = d_buf;
    wPtr = d_buf + d_nsamp;
    rDTE = d_buf_tail - rPtr;
    wDTE = d_buf_tail - wPtr;

    r_ptr_offset = 0;
    w_ptr_offset = 0;
}

DelayLine::DelayLine(const DelayLine& rhs)  {
    d_buf = NULL;
    delay_ms = rhs.delay_ms;
    recalculate();
}

DelayLine::~DelayLine()
{

}

void DelayLine::recalculate() {
    d_nsamp = (uint32_t)(((float)delay_ms / 1000.0f) * (float)samp_rate);

    /*
     * We've changed the amount of delay, so we can leave the write pointer where it
     * is but need to move the read pointer an appropriate distance behind the write
     * pointer, taking into account where the beginning of the delay buffer is.
     */

    if(wPtr - d_nsamp >= d_buf) {
        rPtr = wPtr - d_nsamp;
    }
    else {
        //read pointer needs to wrap around
        //to the end of the circular buffer
        uint64_t head_amt = wPtr - d_buf;
        uint64_t remain_amt = d_nsamp - head_amt;
        rPtr = d_buf_tail - remain_amt;
    }

    rDTE = d_buf_tail - rPtr;
    wDTE = d_buf_tail - wPtr;

}

void DelayLine::config(uint32_t _delay_ms) {
    if(_delay_ms > max_delay) {
        delay_ms = max_delay;
        std::cout << "ERROR: delay of " << std::dec << _delay_ms << " is > than max delay of " << max_delay
        << ".  Delay capped." << std::endl;
    }
    else {
        delay_ms = _delay_ms;
    }
    recalculate();
}

void DelayLine::process(float* in, float* out, uint32_t n_samps) {
    write_samp_buf(in, n_samps);
    read_samp_buf(out, n_samps);
}

void DelayLine::read_samp_buf(float* out, uint32_t n) {
    if(rDTE < n) {
        memcpy(out, rPtr, rDTE * sizeof(float));
        r_ptr_offset = rDTE;
        n -= rDTE;
        rDTE = d_buf_len;
        rPtr = d_buf;
        memcpy(out + r_ptr_offset, rPtr, n * sizeof(float));
        rDTE -= n;
        rPtr += n;
    }
    else {
        memcpy(out, rPtr, n * sizeof(float));
        rPtr += n;
        rDTE -= n;
    }
}

void DelayLine::write_samp_buf(float* in, uint32_t n) {
    if(wDTE < n) {
        memcpy(wPtr, in, wDTE * sizeof(float));
        w_ptr_offset = wDTE;
        n -= wDTE;
        wDTE = d_buf_len;
        wPtr = d_buf;
        memcpy(wPtr, in + w_ptr_offset, n * sizeof(float));
        wDTE -= n;
        wPtr += n;
    }
    else {
        memcpy(wPtr, in, n * sizeof(float));
        wPtr += n;
        wDTE -= n;
    }
}

} //namespace fmsmoov
