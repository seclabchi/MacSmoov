//
//  SignalGeneratorEngine.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 4/1/23.
//

#include "SignalGeneratorEngine.hpp"

#include <iostream>
#include <random>
#include <math.h>

using namespace fmsmoov;

static std::mt19937 rng;
static std::uniform_real_distribution<float> urd(-1.0, 1.0);
static std::normal_distribution<float> nd(0.0,1.0);

SignalGeneratorEngine::SignalGeneratorEngine(uint32_t _f_samp, uint32_t _n_channels) : f_samp(_f_samp), n_channels(_n_channels) {
    volume = 1.0;
    cur_samp = 0;
    type = SINE;
    frequency = 400;
}

SignalGeneratorEngine::~SignalGeneratorEngine() {
    
}

void SignalGeneratorEngine::configure(SIG_GEN_TYPE _type, uint32_t _frequency) {
    type = _type;
    frequency = _frequency;
    
    //period of the triangle wave in samples 1/f * Fs
    period_in_samples = (1.0/(float)frequency) * (float)f_samp;
    
    sawtooth_jump_size = 1.0/period_in_samples;
    triangle_jump_size = 2.0/period_in_samples;
    cur_sample_value = 0.0;
    decrement = false;
}

void SignalGeneratorEngine::get_next_buffers(float* out_buf, uint32_t n_frames) {
    float sin_val = 0;
    
    switch(type) {
    case SINE:
        for(uint32_t i = 0; i < n_frames; i+=2) {
            sin_val = volume * sin(2.0f * M_PI * frequency * (float)(cur_samp/f_samp));
            out_buf[i] = sin_val;
            out_buf[i+1] = sin_val;
            if(cur_samp == f_samp) {
                cur_samp = 1.0f;
            }
            else {
                cur_samp++;
            }
        }
        break;
    case SAWTOOTH:
        for(uint32_t i = 0; i < n_frames; i++) {
            out_buf[i] = volume * cur_sample_value;
            out_buf[i+1] = volume * cur_sample_value;
            cur_sample_value += sawtooth_jump_size;
            if(cur_sample_value >= 0.999999) {
                cur_sample_value = -1.0;
            }
        }
        break;
    case TRIANGLE:
        for(uint32_t i = 0; i < n_frames; i++) {
            out_buf[i] = volume * cur_sample_value;
            out_buf[i+1] = volume * cur_sample_value;
            
            if(cur_sample_value >= 0.999999) {
                decrement = true;
            }
            if(cur_sample_value <= -0.999999) {
                decrement = false;
            }
            
            if(decrement) {
                cur_sample_value -= triangle_jump_size;
            }
            else {
                cur_sample_value += triangle_jump_size;
            }
            
        }
        break;
    case SQUARE:
        break;
    case NOISE_WHITE:
        {
            float rand_val;
            for(uint32_t i = 0; i < n_frames; i+=2) {
                rand_val = volume * urd(rng);
                out_buf[i] = rand_val;
                out_buf[i+1] = rand_val;
            }
        }
        break;
    case NOISE_PINK:
        {
            float white_noise_val;
            float* pink_noise_buf = new float[n_frames/2];
            float b[7];
            float peak_min = 0.0;
            float peak_max = 0.0;
            
            memset(b, 0, 7*sizeof(float));
            
            /*  Apply weighted sum of first order filters to approximate a -10dB/decade
              *  filter.  This is Paul Kellet's "refined" method (a.k.a instrumentation
              *  grade)  It is accurate to within +/-0.05dB above 9.2Hz
              */
            
            for(uint32_t i = 0; i < n_frames/2; i++) {
                white_noise_val = nd(rng);
                b[0] = 0.99886 * b[0] + white_noise_val * 0.0555179;
                b[1] = 0.99332 * b[1] + white_noise_val * 0.0750759;
                b[2] = 0.96900 * b[2] + white_noise_val * 0.1538520;
                b[3] = 0.86650 * b[3] + white_noise_val * 0.3104856;
                b[4] = 0.55000 * b[4] + white_noise_val * 0.5329522;
                b[5] = -0.7616 * b[5] - white_noise_val * 0.0168980;
                pink_noise_buf[i] = b[0] + b[1] + b[2] + b[3] + b[4] + b[5] + b[6] + white_noise_val * 0.5362;
                b[6] = white_noise_val * 0.115926;
            }
            
            for(uint32_t i = 0; i < n_frames/2; i++) {
                out_buf[2*i] = 0.08 * volume * pink_noise_buf[i];
                out_buf[2*i+1] = 0.08 * volume * pink_noise_buf[i];
            }
        
            delete[] pink_noise_buf;
        }
        break;
    case UNKNOWN:
        break;
    }
}

void SignalGeneratorEngine::adjust_volume(float _volume) {
    volume = _volume;
}
