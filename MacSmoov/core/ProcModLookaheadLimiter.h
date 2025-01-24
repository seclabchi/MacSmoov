//
//  ProcModLookaheadLimiter.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 9/20/24.
//

#ifndef ProcModLookaheadLimiter_hpp
#define ProcModLookaheadLimiter_hpp

#include <stdio.h>
#include "ProcessorModule.hpp"

namespace fmsmoov {

class ProcModLookaheadLimiter : public ProcessorModule {
public:
    ProcModLookaheadLimiter(const string& _name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps);
    virtual ~ProcModLookaheadLimiter();
    virtual bool init_impl(CoreConfig* cfg, ProcessorModule* prev_mod, ChannelMap* _channel_map);
    void set_gain_db(float gain_L, float gain_R);
    /* n samps is total interleaved stereo samples
     TODO: Figure this shit out to make it universal. */
    virtual void process();
private:
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

#endif /* ProcModLookaheadLimiter_hpp */
