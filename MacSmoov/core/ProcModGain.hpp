//
//  ProcModGain.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 8/11/23.
//

#ifndef ProcModGain_hpp
#define ProcModGain_hpp

#include <stdio.h>
#include "ProcessorModule.hpp"

namespace fmsmoov {

class ProcModGain : public ProcessorModule {
public:
    ProcModGain(const string& _name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps);
    virtual ~ProcModGain();
    /* n samps is total interleaved stereo samples
     TODO: Figure this shit out to make it universal. */
    virtual void process();
    void set_gain_db(float _gainL, float _gainR);
private:
    AudioBuf* inbuf_main;
    AudioBuf* outbuf_main;
    float m_setgain_db_L;
    float m_setgain_db_R;
    float m_setgain_lin_L;
    float m_setgain_lin_R;
    float rms_l;
    float rms_r;
    float peak_l;
    float peak_r;
    float tmp_samp_mag;
    float out_rms_l, out_rms_r, out_peak_l, out_peak_r;
};

}

#endif /* ProcModLevelMeter_hpp */
