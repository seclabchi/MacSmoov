//
//  ProcMod5bandCrossover.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 8/28/23.
//

#include "ProcMod5bandCrossover.hpp"
#include "sos.h"

namespace fmsmoov {

ProcMod5bandCrossover::ProcMod5bandCrossover(const string& _name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps) : ProcessorModule (_name, _f_samp, _n_channels, _n_samps) {

    this->set_out_buf(0, new AudioBuf(AudioBufType::REAL, "OUT_L", _n_samps));
    this->set_out_buf(1, new AudioBuf(AudioBufType::REAL, "OUT_R", _n_samps));
    
    this->set_out_buf(2, new AudioBuf(AudioBufType::REAL, "OUT_B1_L", _n_samps));
    this->set_out_buf(3, new AudioBuf(AudioBufType::REAL, "OUT_B1_R", _n_samps));
    this->set_out_buf(4, new AudioBuf(AudioBufType::REAL, "OUT_B2_L", _n_samps));
    this->set_out_buf(5, new AudioBuf(AudioBufType::REAL, "OUT_B2_R", _n_samps));
    this->set_out_buf(6, new AudioBuf(AudioBufType::REAL, "OUT_B3_L", _n_samps));
    this->set_out_buf(7, new AudioBuf(AudioBufType::REAL, "OUT_B3_R", _n_samps));
    this->set_out_buf(8, new AudioBuf(AudioBufType::REAL, "OUT_B4_L", _n_samps));
    this->set_out_buf(9, new AudioBuf(AudioBufType::REAL, "OUT_B4_R", _n_samps));
    this->set_out_buf(10, new AudioBuf(AudioBufType::REAL, "OUT_B5_L", _n_samps));
    this->set_out_buf(11, new AudioBuf(AudioBufType::REAL, "OUT_B5_R", _n_samps));
    
    b1out = new float*[2];
    b1out[0] = this->get_out_buf(2)->getbuf();
    b1out[1] = this->get_out_buf(3)->getbuf();
    b2out = new float*[2];
    b2out[0] = this->get_out_buf(4)->getbuf();
    b2out[1] = this->get_out_buf(5)->getbuf();
    b3out = new float*[2];
    b3out[0] = this->get_out_buf(6)->getbuf();
    b3out[1] = this->get_out_buf(7)->getbuf();
    b4out = new float*[2];
    b4out[0] = this->get_out_buf(8)->getbuf();
    b4out[1] = this->get_out_buf(9)->getbuf();
    b5out = new float*[2];
    b5out[0] = this->get_out_buf(10)->getbuf();
    b5out[1] = this->get_out_buf(11)->getbuf();
    
    crossover = new Crossover_5_band(n_samps);
    crossover->band_enable(true, true, true, true, true);
}

ProcMod5bandCrossover::~ProcMod5bandCrossover() {
    delete[] b1out;
    delete[] b2out;
    delete[] b3out;
    delete[] b4out;
    delete[] b5out;
}

void ProcMod5bandCrossover::process() {
    if(this->bypass) {
        memcpy(this->outbufs[0]->getbuf(), this->inbufs[0]->getbuf(), n_samps * sizeof(float));
        memcpy(this->outbufs[1]->getbuf(), this->inbufs[1]->getbuf(), n_samps * sizeof(float));
        return;
    }
    
    //first two buffers will be a stereo passthrough
    inL = this->inbufs[0]->getbuf();
    inR = this->inbufs[1]->getbuf();
    outL = this->outbufs[0]->getbuf();
    outR = this->outbufs[1]->getbuf();
    
    crossover->process(inL, inR, b1out, b2out, b3out, b4out, b5out, n_samps);
    
    //passthrough the input to the first two output bufs
    memcpy(this->get_out_buf(0)->getbuf(), this->get_in_buf(0)->getbuf(), n_samps * sizeof(float));
    memcpy(this->get_out_buf(1)->getbuf(), this->get_in_buf(1)->getbuf(), n_samps * sizeof(float));
    
}

void ProcMod5bandCrossover::band_enable(bool b1, bool b2, bool b3, bool b4, bool b5) {
    crossover->band_enable(b1, b2, b3, b4, b5);
}

}
