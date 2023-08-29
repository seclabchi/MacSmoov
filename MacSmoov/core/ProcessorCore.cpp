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
        .release_master = 8.0,
        .release_bass = 8.0,
        .gate_thresh = -65.0,
        .bass_coupling = 0.7,
        .window_size = -3.0,
        .window_release = 30,
        .ratio = 4.0,
        .bass_thresh = 0.0,
        .idle_gain = 0.0,
        .attack_master = 0.50,
        .attack_bass = 1.0,
        .post_gain = 20.0
    };
    
    proc_mod_2band_agc = new ProcMod2BandAGC("2BAND_AGC_IN", f_samp, n_channels, n_samp);
    proc_mod_2band_agc->set_in_buf(0, proc_mod_level_main_in->get_out_buf(0));
    proc_mod_2band_agc->set_in_buf(1, proc_mod_level_main_in->get_out_buf(1));
    proc_mod_2band_agc->setup(agc_params);
    
    proc_mod_5b_crossover = new ProcMod5bandCrossover("5BAND_CROSSOVER", f_samp, n_channels, n_samp);
    proc_mod_5b_crossover->set_in_buf(0, proc_mod_2band_agc->get_out_buf(0), "IN_L");
    proc_mod_5b_crossover->set_in_buf(1, proc_mod_2band_agc->get_out_buf(1), "IN_R");
    proc_mod_5b_crossover->band_enable(true, true, true, true, true);
    
    COMPRESSOR_PARAMS comp_params_b1 = {
        .drive = -36.0,
        .release = .1,
        .gate_thresh = -60.0,
        .use_coupling = false,
        .coupling = 0.0,
        .window_size = 3.0,
        .window_release = 2.0,
        .ratio = 1,
        .idle_gain = 0.0,
        .attack = 0.050,
        .post_gain = 0.0
    };
    
    COMPRESSOR_PARAMS comp_params_b2 = {
        .drive = -36.0,
        .release = .1,
        .gate_thresh = -60.0,
        .use_coupling = false,
        .coupling = 0.0,
        .window_size = 3.0,
        .window_release = 2.0,
        .ratio = 1,
        .idle_gain = 0.0,
        .attack = 0.040,
        .post_gain = 0.0
    };
    
    COMPRESSOR_PARAMS comp_params_b3 = {
        .drive = -40.0,
        .release = .1,
        .gate_thresh = -60.0,
        .use_coupling = false,
        .coupling = 0.0,
        .window_size = 3.0,
        .window_release = 2.0,
        .ratio = 1,
        .idle_gain = 0.0,
        .attack = 0.032,
        .post_gain = 0.0
    };
    
    COMPRESSOR_PARAMS comp_params_b4 = {
        .drive = -40.0,
        .release = .1,
        .gate_thresh = -60.0,
        .use_coupling = false,
        .coupling = 0.0,
        .window_size = 3.0,
        .window_release = 2.0,
        .ratio = 1,
        .idle_gain = 0.0,
        .attack = 0.016,
        .post_gain = 0.0
    };
    
    COMPRESSOR_PARAMS comp_params_b5 = {
        .drive = -40.0,
        .release = .1,
        .gate_thresh = -60.0,
        .use_coupling = false,
        .coupling = 0.0,
        .window_size = 3.0,
        .window_release = 2.0,
        .ratio = 1,
        .idle_gain = 0.0,
        .attack = 0.008,
        .post_gain = 0.0
    };
    
    MULTIBAND_PARAMS mb_params = {
        .band_params[0] = comp_params_b1,
        .band_params[1] = comp_params_b2,
        .band_params[2] = comp_params_b3,
        .band_params[3] = comp_params_b4,
        .band_params[4] = comp_params_b5
    };
    
    proc_mod_5b_compressor = new ProcMod5bandCompressor("5BAND_COMPRESSOR", f_samp, n_channels, n_samp);
    proc_mod_5b_compressor->set_in_buf(0, proc_mod_5b_crossover->get_out_buf(0), "IN_L");
    proc_mod_5b_compressor->set_in_buf(1, proc_mod_5b_crossover->get_out_buf(1), "IN_R");
    proc_mod_5b_compressor->set_in_buf(2, proc_mod_5b_crossover->get_out_buf(2), "IN_B1_L");
    proc_mod_5b_compressor->set_in_buf(3, proc_mod_5b_crossover->get_out_buf(3), "IN_B1_R");
    proc_mod_5b_compressor->set_in_buf(4, proc_mod_5b_crossover->get_out_buf(4), "IN_B2_L");
    proc_mod_5b_compressor->set_in_buf(5, proc_mod_5b_crossover->get_out_buf(5), "IN_B2_R");
    proc_mod_5b_compressor->set_in_buf(6, proc_mod_5b_crossover->get_out_buf(6), "IN_B3_L");
    proc_mod_5b_compressor->set_in_buf(7, proc_mod_5b_crossover->get_out_buf(7), "IN_B3_R");
    proc_mod_5b_compressor->set_in_buf(8, proc_mod_5b_crossover->get_out_buf(8), "IN_B4_L");
    proc_mod_5b_compressor->set_in_buf(9, proc_mod_5b_crossover->get_out_buf(9), "IN_B5_R");
    proc_mod_5b_compressor->set_in_buf(10, proc_mod_5b_crossover->get_out_buf(10), "IN_B5_L");
    proc_mod_5b_compressor->set_in_buf(11, proc_mod_5b_crossover->get_out_buf(11), "IN_B5_R");
    proc_mod_5b_compressor->setup(mb_params);
    proc_mod_5b_compressor->set_bypass(false);
    
    m_loglin = new LogLinConverter(LogLinConversionType::LOG_TO_LIN);
    m_linlog = new LogLinConverter(LogLinConversionType::LIN_TO_LOG);
}


/* n_samp is the number of samples per channel, so each in_L and in_R will have this number of samples.
   Doing the first cut of this w/AUHAL on OSX so hopefully it's easy to get non-interleaved samples with JACK/ALSA
 */
void ProcessorCore::process(float* in_L, float* in_R, float* out_L, float* out_R, uint32_t n_samp) {
    proc_mod_gain_main_in->update_in_buf_ref(0, in_L);
    proc_mod_gain_main_in->update_in_buf_ref(1, in_R);
    
    proc_mod_5b_compressor->update_out_buf_ref(0, out_L);
    proc_mod_5b_compressor->update_out_buf_ref(1, out_R);
    
    proc_mod_gain_main_in->process();
    proc_mod_level_main_in->process();
    //proc_mod_stereo_enhance->process();
    proc_mod_2band_agc->process();
    proc_mod_5b_crossover->process();
    proc_mod_5b_compressor->process();
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

void ProcessorCore::get5bandCompressorGainReduction(float** _bands_gr) {
    proc_mod_5b_compressor->read(_bands_gr);
}
