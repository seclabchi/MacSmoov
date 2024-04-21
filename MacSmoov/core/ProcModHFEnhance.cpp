//
//  ProcModHFEnhance.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 9/16/23.
//

#include "ProcModHFEnhance.hpp"
#include "sos.h"
#include <iostream>

using namespace std;

namespace fmsmoov {
/*
 * 5 kHz 3rd order highpass filter, and a bandpass filter that roughly approximates the
 * Fletcher-Munson "equal loudness" curve at a level of 70 phon.
 
 These are 48 kHz sample rate values!
 
>> third_order_hp

sos =

   1.0000  -2.0000   1.0000   1.0000  -1.2164   0.5333
   1.0000  -1.0000        0   1.0000  -0.4931        0

g = 0.5132
>> fletcher_munson_70phons

sos =

   1.0000e+00   2.0000e+00   9.9999e-01   1.0000e+00  -1.3008e+00   6.7945e-01
   1.0000e+00  -2.0000e+00   9.9999e-01   1.0000e+00  -1.4646e+00   5.7735e-01
   1.0000e+00  -1.6058e-07  -1.0000e+00   1.0000e+00  -1.8428e+00   8.8307e-01

g = 0.011325
*/

ProcModHFEnhance::ProcModHFEnhance(const string& _name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps) : ProcessorModule (_name, _f_samp, _n_channels, _n_samps) {
    SOS sos110_1(0.5132, 1.0000, -2.0000, 1.0000, 1.0000, -1.2164, 0.5333);
    SOS sos110_2(0.5132, 1.0000, -1.0000, 0, 1.0000, -0.4931, 0);
    SOS sos120_1(0.011325, 1.0000e+00, 2.0000e+00, 9.9999e-01, 1.0000e+00, -1.3008e+00, 6.7945e-01);
    SOS sos120_2(0.011325, 1.0000e+00, -2.0000e+00, 9.9999e-01, 1.0000e+00, -1.4646e+00, 5.7735e-01);
    SOS sos120_3(0.011325, 1.0000e+00, -1.6058e-07, -1.0000e+00, 1.0000e+00, -1.8428e+00, 8.8307e-01);
    
    filt_110L = new Filter3rdOrder(&sos110_1, &sos110_2, _n_samps);
    filt_110R = new Filter3rdOrder(&sos110_1, &sos110_2, _n_samps);
    filt_120L = new FilterEL(&sos120_1, &sos120_2, &sos120_3, _n_samps);
    filt_120R = new FilterEL(&sos120_1, &sos120_2, &sos120_3, _n_samps);
    
    buf_130L = new float[_n_samps]();
    buf_130R = new float[_n_samps]();
    buf_140L = new float[_n_samps]();
    buf_140R = new float[_n_samps]();
    
    rms_150L = new RunningRMS(0.010f, 48000, _n_samps, 0.001, 0.030);
    rms_150R = new RunningRMS(0.010f, 48000, _n_samps, 0.001, 0.030);
    rms_160L = new RunningRMS(0.010f, 48000, _n_samps, 0.001, 0.030);
    rms_160R = new RunningRMS(0.010f, 48000, _n_samps, 0.001, 0.030);
    
    buf_170L = new float[_n_samps]();
    buf_170R = new float[_n_samps]();
    buf_180L = new float[_n_samps]();
    buf_180R = new float[_n_samps]();
    
    master_outL = new float[_n_samps]();
    memset(master_outL, 0, _n_samps*sizeof(float));
    master_outR = new float[_n_samps]();
    memset(master_outR, 0, _n_samps*sizeof(float));
    
    this->set_out_buf(0, new AudioBuf(AudioBufType::REAL, "OUT_L", _n_samps));
    this->set_out_buf(1, new AudioBuf(AudioBufType::REAL, "OUT_R", _n_samps));
    
    linlogL = new LogLinConverter(LogLinConversionType::LIN_TO_LOG);
    linlogR = new LogLinConverter(LogLinConversionType::LIN_TO_LOG);
}

ProcModHFEnhance::~ProcModHFEnhance() {
    delete[] master_outL;
    delete[] master_outR;
    
    delete filt_110L;
    delete filt_110R;
    delete filt_120L;
    delete filt_120R;
    
    delete[] buf_130L;
    delete[] buf_130R;
    delete[] buf_140L;
    delete[] buf_140R;
    
    delete[] buf_170L;
    delete[] buf_170R;
    delete[] buf_180L;
    delete[] buf_180R;
}

bool ProcModHFEnhance::init_impl(CoreConfig* cfg, ProcessorModule* prev_mod, ChannelMap* _channel_map) {
    if(nullptr != cfg) {
        //TODO config
    }
    if((nullptr != _channel_map) && (nullptr != prev_mod)) {
        for(CHANNEL_MAP_ELEMENT e : _channel_map->the_map)
        {
            this->set_in_buf(e.this_chan, prev_mod->get_out_buf(e.in_chan));
        }
    }
    
    return true;
}

void ProcModHFEnhance::process() {
    uint32_t n_samps = this->get_n_samps();
    float* inL = this->get_in_buf(0)->getbuf();
    float* inR = this->get_in_buf(1)->getbuf();
    
    if(this->bypass) {
        memcpy(this->get_out_buf(0)->getbuf(), this->get_in_buf(0)->getbuf(), n_samps * sizeof(float));
        memcpy(this->get_out_buf(1)->getbuf(), this->get_in_buf(1)->getbuf(), n_samps * sizeof(float));
        return;
    }
    
    filt_110L->process(this->get_in_buf(0)->getbuf(), buf_130L);
    filt_110R->process(this->get_in_buf(1)->getbuf(), buf_130R);
    filt_120L->process(this->get_in_buf(0)->getbuf(), buf_140L);
    filt_120R->process(this->get_in_buf(1)->getbuf(), buf_140R);
    
    rms_150L->process(buf_130L, buf_170L);
    rms_150R->process(buf_130R, buf_170R);
    rms_160L->process(buf_140L, buf_180L);
    rms_160R->process(buf_140R, buf_180R);
    
    for(uint32_t i = 0; i < n_samps; i++) {
        master_outL[i] = inL[i] * 100 * (buf_170L[i] - buf_180L[i]);
        master_outR[i] = inR[i] * 100 * (buf_170R[i] - buf_180R[i]);
    }
    
    memcpy(this->get_out_buf(0)->getbuf(), master_outL, n_samps * sizeof(float));
    memcpy(this->get_out_buf(1)->getbuf(), master_outR, n_samps * sizeof(float));
    
}

}
