//
//  ProcessorModule.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 7/9/23.
//

#include <iostream>
#include <sstream>
#include <cstdint>
#include "ProcessorModule.hpp"

using namespace std;

namespace fmsmoov {

ProcessorModule::ProcessorModule(const string& _name, uint32_t _f_samp, uint32_t _n_channels, uint32_t _n_samps) : name(_name), f_samp(_f_samp), n_channels(_n_channels),
    n_samps(_n_samps) {
        memset(inbufs, 0, MAX_INPUT_BUFFERS * sizeof(AudioBuf*));
        memset(outbufs, 0, MAX_OUTPUT_BUFFERS * sizeof(AudioBuf*));
        bypass = false;
}

ProcessorModule::~ProcessorModule() {
    for(uint32_t i = 0; i < MAX_INPUT_BUFFERS; i++) {
        if(inbufs[i] != NULL) {
            delete inbufs[i];
        }
    }
    
    for(uint32_t i = 0; i < MAX_OUTPUT_BUFFERS; i++) {
        if(outbufs[i] != NULL) {
            delete outbufs[i];
        }
    }
}

void ProcessorModule::set_bypass(bool _bypass) {
    bypass = _bypass;
}

bool ProcessorModule::get_bypass() {
    return bypass;
}

uint32_t ProcessorModule::get_f_samp() {
    return f_samp;
}

uint32_t ProcessorModule::get_n_channels() {
    return n_channels;
}

uint32_t ProcessorModule::get_n_samps() {
    return n_samps;
}

const string& ProcessorModule::get_name() {
    return name;
}

AudioBuf* ProcessorModule::get_in_buf(uint32_t buf_index) {
    if(NULL == inbufs[buf_index]) {
        throw std::invalid_argument("get_in_buf index is NULL.");
    }
    return (inbufs[buf_index]);
}

AudioBuf* ProcessorModule::get_out_buf(uint32_t buf_index) {
    if(NULL == outbufs[buf_index]) {
        throw std::invalid_argument("get_out_buf index is NULL.");
    }
    return (outbufs[buf_index]);
}

void ProcessorModule::set_in_buf(uint32_t buf_index, AudioBuf* abref, const string& new_name) {
    if(NULL == abref) {
        throw std::invalid_argument("Can't set in_buf with NULL reference.");
    }
    else {
        if(NULL == inbufs[buf_index]) {
            cout << "Creating in_buf at position " << buf_index << " in " << __FILE__ << ":" << __LINE__ << " for module " << name << endl;
            inbufs[buf_index] = new AudioBuf(*abref);
            inbufs[buf_index]->set_type(AudioBufType::REFERENCE);
            if(new_name.compare("")) {
                inbufs[buf_index]->set_name(new_name);
            }
        }
        else {
            cout << "Overwriting in_buf at position " << buf_index << " in " << __FILE__ << ":" << __LINE__ << " for module " << name << endl;
            inbufs[buf_index] = abref;
            inbufs[buf_index]->set_type(AudioBufType::REFERENCE);
            if(new_name.compare("")) {
                inbufs[buf_index]->set_name(new_name);
            }
        }
    }
}

void ProcessorModule::set_out_buf(uint32_t buf_index, AudioBuf* abref) {
    if(NULL == abref) {
        throw std::invalid_argument("Can't set out_buf with NULL reference.");
    }
    else {
        if(NULL == outbufs[buf_index]) {
            cout << "Creating out_buf at position " << buf_index << " in " << __FILE__ << ":" << __LINE__ << " for module " << name << endl;
            outbufs[buf_index] = new AudioBuf(*abref);
        }
        else {
            if(AudioBufType::REAL == outbufs[buf_index]->get_type()) {
                delete[] outbufs[buf_index]->getbuf();
            }
            cout << "Overwriting out_buf at position " << buf_index << " in " << __FILE__ << ":" << __LINE__ << " for module " << name << endl;
            outbufs[buf_index] = abref;
        }
    }
}

void ProcessorModule::update_in_buf_ref(uint32_t buf_index, float* bufref) {
    if(NULL == inbufs[buf_index]) {
        throw std::invalid_argument("Can't update inbuf reference.  Buffer is NULL.");
    }
    
    if(NULL == bufref) {
        throw std::invalid_argument("Trying to update input buf ref with NULL reference.");
    }
    
    inbufs[buf_index]->setbuf(bufref);
}

void ProcessorModule::update_out_buf_ref(uint32_t buf_index, float* bufref) {
    if(NULL == outbufs[buf_index]) {
        throw std::invalid_argument("Can't update outbuf reference.  Buffer is NULL.");
    }
    if(NULL == bufref) {
        throw std::invalid_argument("Trying to update output buf ref with NULL reference.");
    }
    
    outbufs[buf_index]->setbuf(bufref);
}

void ProcessorModule::deinterleave(float* inbuf, float* buf_in_L, float* buf_in_R) {
    uint32_t n = 0;
    for(uint32_t i = 0; i < n_samps/2; i++) {
        buf_in_L[i] = inbuf[n];
        buf_in_R[i] = inbuf[n+1];
        n+=2;
    }
}

void ProcessorModule::interleave(float* buf_in_L, float* buf_in_R, float* outbuf) {
    uint32_t n = 0;
    for(uint32_t i = 0; i < n_samps/2; i++) {
        outbuf[n] = buf_in_L[i];
        outbuf[n+1] = buf_in_R[i];
        n+=2;
    }
}

}
