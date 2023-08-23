//
//  ProcessorCore.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 4/15/23.
//

#include "ProcessorCore.hpp"
#include "LogLinConverter.hpp"
#include <string>
#include <iostream>

using namespace fmsmoov;
using namespace std;

/* n_samp is number of samples PER CHANNEL */
ProcessorCore::ProcessorCore(uint32_t _f_samp, uint32_t _n_channels, uint32_t _n_samp) : f_samp(_f_samp), n_channels(_n_channels), n_samp(_n_samp) {
    proc_mod_gain_main_in = new ProcModGain("GAIN_MAIN_IN", f_samp, n_channels, n_samp);
    
    proc_mod_level_main_in = new ProcModLevelMeter("LEVEL_MAIN_IN", f_samp, n_channels, n_samp);
    AudioBuf* gmoL = proc_mod_gain_main_in->get_out_buf(0);
    AudioBuf* gmoR = proc_mod_gain_main_in->get_out_buf(1);
    proc_mod_level_main_in->set_in_buf(0, gmoL, "IN_L");
    proc_mod_level_main_in->set_in_buf(1, gmoR, "IN_R");
    
    //proc_mod_stereo_enhance = new ProcModStereoEnhance("STEREO_ENHANCER", f_samp, n_channels, n_samp);
    //AudioBuf* lmoL = proc_mod_level_main_in->get_out_buf(0);
    //AudioBuf* lmoR = proc_mod_level_main_in->get_out_buf(1);
    //proc_mod_stereo_enhance->set_in_buf(0, lmoL, "IN_L");
    //proc_mod_stereo_enhance->set_in_buf(1, lmoR, "IN_R");
    
    proc_mod_2band_agc = new ProcMod2BandAGC("2BAND_AGC_IN", f_samp, n_channels, n_samp);
    proc_mod_2band_agc->set_in_buf(0, proc_mod_level_main_in->get_out_buf(0));
    proc_mod_2band_agc->set_in_buf(1, proc_mod_level_main_in->get_out_buf(1));
    
    /*
    typedef struct {
        float target;
        float tatt;
        float trel;
        float fixed_gain;
        float gate_thresh;
        float gate_hold_time;
    } AGC_PARAMS;
    */
     
    AGC_PARAMS agc_params_lo = {-40.0, 20.0, 20.0, 12.0, -50.0, 5.0, 0.125};
    AGC_PARAMS agc_params_hi = {-40.0, 20.0, 20.0, 20.0, -40.0, 5.0, 0.0};
    
    proc_mod_2band_agc->setup(agc_params_lo, agc_params_hi);
    
    m_loglin = new LogLinConverter(LogLinConversionType::LOG_TO_LIN);
    m_linlog = new LogLinConverter(LogLinConversionType::LIN_TO_LOG);
}


/* n_samp is the number of samples per channel, so each in_L and in_R will have this number of samples.
   Doing the first cut of this w/AUHAL on OSX so hopefully it's easy to get non-interleaved samples with JACK/ALSA
 */
void ProcessorCore::process(float* in_L, float* in_R, float* out_L, float* out_R, uint32_t n_samp) {
    proc_mod_gain_main_in->update_in_buf_ref(0, in_L);
    proc_mod_gain_main_in->update_in_buf_ref(1, in_R);
    
    proc_mod_2band_agc->update_out_buf_ref(0, out_L);
    proc_mod_2band_agc->update_out_buf_ref(1, out_R);
    
    proc_mod_gain_main_in->process();
    proc_mod_level_main_in->process();
    //proc_mod_stereo_enhance->process();
    proc_mod_2band_agc->process();
}

void ProcessorCore::get_main_in_levels(float* lrms, float* rrms, float* lpeak, float* rpeak) {
    proc_mod_level_main_in->get_levels_db(lrms, rrms, lpeak, rpeak);
}

void ProcessorCore::set_main_in_gain_db(float loggain_l, float loggain_r) {
    proc_mod_gain_main_in->set_gain_db(loggain_l, loggain_r);
}

void ProcessorCore::get2bandAGCGainReduction(float* gainReduct2blo, float* gainReduct2bhi, bool* gateOpenLo, bool* gateOpenHi) {
    proc_mod_2band_agc->read(gainReduct2blo, gainReduct2bhi, gateOpenLo, gateOpenHi);
    //cout << "GainredL=" << *gainReduct2blo << " H=" << *gainReduct2bhi << " GATES " << gate_open_lo << "|" << gate_open_hi << endl;
}
