//
//  ProcMod2BandAGC.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 8/12/23.
//

#include "ProcMod2BandAGC.hpp"
#include "sos.h"

namespace fmsmoov {

/* 44100 Hz sample rate, 150 Hz LO/HI pass second order sections
   NO GAIN SHOULD BE APPLIED
 soslo =

    2.8332e-05   5.6663e-05   2.8332e-05   1.0000e+00  -1.9849e+00   9.8500e-01

 soshi =

    0.9925  -1.9849   0.9925   1.0000  -1.9849   0.9850
 */

ProcMod2BandAGC::ProcMod2BandAGC(const string& _name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps) : ProcessorModule (_name, _f_samp, _n_channels, _n_samps) {
    SOS soslo(1.0, 2.8332e-05, 5.6663e-05, 2.8332e-05, 1.0, -1.9849e+00, 9.8500e-01);
    SOS soshi(1.0, 0.9925, -1.9849, 0.9925, 1.0000, -1.9849, 0.9850);
    filt_lo_L = new FilterLR4(&soslo, _n_samps);
    filt_lo_R = new FilterLR4(&soslo, _n_samps);
    filt_hi_L = new FilterLR4(&soshi, _n_samps);
    filt_hi_R = new FilterLR4(&soshi, _n_samps);
    buf_lin_in = new AudioBuf(AudioBufType::REFERENCE, string("LINEAR_IN"), _n_samps, NULL);
    buf_db_in = new AudioBuf(AudioBufType::REFERENCE, string("DB_IN"), _n_samps, NULL);
    buf_out = new AudioBuf(AudioBufType::REAL, string("OUT"), _n_samps, NULL);
    this->set_in_buf(0, buf_lin_in);
    this->set_in_buf(1, buf_db_in);
    this->set_out_buf(0, buf_out);
    
    buf_lo_filtL = new float[_n_samps/2];
    buf_lo_filtR = new float[_n_samps/2];
    buf_hi_filtL = new float[_n_samps/2];
    buf_hi_filtR = new float[_n_samps/2];
    
    buf_in_L = new float[_n_samps/2];
    buf_in_R = new float[_n_samps/2];
    
    testbuf = new float[_n_samps];
}

ProcMod2BandAGC::~ProcMod2BandAGC() {
    delete filt_lo_L;
    delete filt_lo_R;
    delete filt_hi_L;
    delete filt_hi_R;
    delete buf_lin_in;
    delete buf_db_in;
    delete buf_out;
    
    delete buf_in_L;
    delete buf_in_R;
    
    delete buf_lo_filtL;
    delete buf_lo_filtR;
    delete buf_hi_filtL;
    delete buf_hi_filtR;
}

void ProcMod2BandAGC::process() {
    
    float* testInBuf = new float[this->get_n_samps()];
    float* testOutBuf = new float[this->get_n_samps()];
    memcpy(testInBuf, this->get_in_buf(0)->getbuf(), this->get_n_samps() * sizeof(float));
    //deinterleave(testInBuf, buf_in_L, buf_in_R);
    //filt_lo_L->process(buf_in_L, buf_lo_filtL);
    //filt_lo_R->process(buf_in_R, buf_lo_filtR);
    //filt_hi_L->process(buf_in_L, buf_hi_filtL);
    //filt_hi_R->process(buf_in_R, buf_hi_filtR);
    //interleave(buf_hi_filtL, buf_hi_filtR, testOutBuf);
    memcpy(testOutBuf, testInBuf, this->get_n_samps() * sizeof(float));
    memcpy(this->get_out_buf(0)->getbuf(), testOutBuf, this->get_n_samps() * sizeof(float));
    //memcpy(this->get_out_buf(0)->getbuf(), this->get_in_buf(0)->getbuf(), this->get_n_samps()*sizeof(float));
    delete[] testInBuf;
    delete[] testOutBuf;
}

}
