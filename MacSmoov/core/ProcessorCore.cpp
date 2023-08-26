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
    
    /*
     typedef struct {
         bool enabled;  //off-on
         float drive;  //-10...+25 dB
         float release_master;  //0.5...20 dB/s
         float release_bass;    //1...10 dB/s
         float gate_thresh;  //-80...-15 dB
         float bass_coupling;  //0-100%
         float window_size;  //-25...0 dB
         float window_release;  //0.5...20 dB
         float ratio;  //infinity:1....2:1
         float bass_thresh;  //-12...2.5 dB
         float idle_gain;  //-10...+10 dB
         float attack_master;  //0.2...6 seconds
         float attack_bass;   //1...10 seconds
     } AGC_PARAMS;
    */
     
    AGC_PARAMS agc_params = {
        .enabled = true,
        .drive = -40.0,
        .release_master = 12.0,
        .release_bass = 12.0,
        .gate_thresh = -50.0,
        .bass_coupling = 0.7,
        .window_size = -3.0,
        .window_release = 30,
        .ratio = 2.0,
        .bass_thresh = 0.0,
        .idle_gain = 0.0,
        .attack_master = 1.5,
        .attack_bass = 3.0,
        .post_gain = 10.0
    };
    
    proc_mod_2band_agc = new ProcMod2BandAGC("2BAND_AGC_IN", f_samp, n_channels, n_samp, agc_params);
    proc_mod_2band_agc->set_in_buf(0, proc_mod_level_main_in->get_out_buf(0));
    proc_mod_2band_agc->set_in_buf(1, proc_mod_level_main_in->get_out_buf(1));
    
    
    
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
