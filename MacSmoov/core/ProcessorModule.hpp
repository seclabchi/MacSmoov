//
//  ProcessorModule.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 7/9/23.
//

#ifndef ProcessorModule_hpp
#define ProcessorModule_hpp

#include <stdio.h>
#include <cstdint>
#include <vector>
#include <string>
#include "core_utils/AudioBuf.hpp"

#define MAX_INPUT_BUFFERS 24
#define MAX_OUTPUT_BUFFERS 24

namespace fmsmoov {

using namespace std;

class ProcessorModule {
public:
    /* _buf_size is number of samples per channel */
    ProcessorModule(const string& _name, uint32_t _f_samp, uint32_t _n_channels, uint32_t _n_samps);
    virtual ~ProcessorModule();
    /* A "frame" is a stereo pair of float samples. */
    virtual void process() = 0;
    void set_bypass(bool _bypass);
    bool get_bypass();
    uint32_t get_f_samp();
    uint32_t get_n_channels();
    uint32_t get_n_samps();
    const string& get_name();
    AudioBuf* get_in_buf(uint32_t buf_index);
    AudioBuf* get_out_buf(uint32_t buf_index);
    void set_in_buf(uint32_t buf_index, AudioBuf* abref, const string& new_name="");
    void set_out_buf(uint32_t buf_index, AudioBuf* abref);
    void update_in_buf_ref(uint32_t buf_index, float* bufref);
    void update_out_buf_ref(uint32_t buf_index, float* bufref);
    void deinterleave(float* buf_lin_in, float* buf_in_L, float* buf_in_R);
    void interleave(float* buf_in_L, float* buf_in_R, float* outbuf);
protected:
    size_t get_inbuf_count();
    size_t get_outbuf_count();
    bool bypass;
    string name;
    uint32_t f_samp;
    uint8_t n_channels;
    uint32_t n_samps;
    AudioBuf* inbufs[MAX_INPUT_BUFFERS];
    AudioBuf* outbufs[MAX_OUTPUT_BUFFERS];
private:
    
};

}

#endif /* ProcessorModule_hpp */
