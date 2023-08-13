//
//  AudioBuf.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 8/12/23.
//

#include "AudioBuf.hpp"
#include <stdexcept>
#include <string>
#include <iostream>

using namespace std;

namespace fmsmoov {

AudioBuf::AudioBuf(AudioBufType _type, const string& _name, uint32_t _n_samps, AudioBuf* _refsrc) : type(_type), name(_name), n_samps(_n_samps) {
    if(AudioBufType::REFERENCE == type) {
        if(NULL == _refsrc) {
            cout << "WARNING: Creating REFERENCE AudioBuf " << _name << " with a NULL reference.  I hope that this is the first module in the chain." << endl;
            buf = NULL;
        }
        else {
            buf = _refsrc->getbuf();
        }
        
    }
    else if(AudioBufType::REAL == type) {
        if(NULL != _refsrc) {
            throw std::invalid_argument("Can't pass a buffer reference to a REAL audiobuf.");
        }
        else {
            buf = new float[n_samps];
        }
    }
    else {
        throw new std::invalid_argument("Unknown buffer type.");
    }
}

AudioBuf::AudioBuf(const AudioBuf& rhs) {
    this->type = rhs.type;
    this->name = rhs.name;
    this->n_samps = rhs.n_samps;
    this->buf = rhs.buf;
}

void AudioBuf::operator = (const AudioBuf& rhs) {
    this->type = rhs.type;
    this->name = rhs.name;
    this->n_samps = rhs.n_samps;
    this->buf = rhs.buf;
}

AudioBuf::~AudioBuf() {
    if(AudioBufType::REAL == type) {
        delete[] buf;
    }
}

void AudioBuf::get_name(string& _name) {
    _name = name;
}

void AudioBuf::set_name(const string& _name) {
    name = _name;
}


float* AudioBuf::getbuf() {
    return buf;
}

void AudioBuf::setbuf(float* ref) {
    buf = ref;
}

AudioBufType AudioBuf::get_type() {
    return type;
}

void AudioBuf::set_type(AudioBufType _type) {
    type = _type;
}

void AudioBuf::write(float* in, uint32_t _n_samps) {
    if(_n_samps != n_samps) {
        throw std::invalid_argument("Wrong number of samples in buffer to write");
    }
    
    memcpy(buf, in, n_samps * sizeof(float));
}

float* AudioBuf::read(float* out, uint32_t* _n_samps) {
    if(*_n_samps != n_samps) {
        throw std::invalid_argument("Wrong number of samples in buffer to read");
    }
    
    memcpy(out, buf, n_samps*sizeof(float));
    
    return out;
}

}
