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
    
    master_bypass = false;
    
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
        .release_master = 3.0,
        .release_bass = 3.0,
        .gate_thresh = -55.0,
        .bass_coupling = 0.3,
        .window_size = -3.0,
        .window_release = 60,
        .ratio = 4.5,
        .bass_thresh = 0.0,
        .idle_gain = 0.0,
        .attack_master = 0.200,
        .attack_bass = 0.350,
        .post_gain = 17.0
    };
    
    proc_mod_2band_agc = new ProcMod2BandAGC("2BAND_AGC_IN", f_samp, n_channels, n_samp);
    proc_mod_2band_agc->set_in_buf(0, proc_mod_level_main_in->get_out_buf(0));
    proc_mod_2band_agc->set_in_buf(1, proc_mod_level_main_in->get_out_buf(1));
    proc_mod_2band_agc->setup(agc_params);
    proc_mod_2band_agc->set_bypass(false);
    
    proc_mod_5b_crossover = new ProcMod5bandCrossover("5BAND_CROSSOVER", f_samp, n_channels, n_samp);
    proc_mod_5b_crossover->set_in_buf(0, proc_mod_2band_agc->get_out_buf(0), "IN_L");
    proc_mod_5b_crossover->set_in_buf(1, proc_mod_2band_agc->get_out_buf(1), "IN_R");
    proc_mod_5b_crossover->band_enable(true, true, true, true, true);
    

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
    //proc_mod_5b_compressor->setup(mb_params);  /*SHOULD BE PASSED INTO CORE BEFORE PROCESSING STARTS*/
    proc_mod_5b_compressor->set_bypass(false);
    
    m_loglin = new LogLinConverter(LogLinConversionType::LOG_TO_LIN);
    m_linlog = new LogLinConverter(LogLinConversionType::LIN_TO_LOG);
}

/* n_samp is the number of samples per channel, so each in_L and in_R will have this number of samples.
   Doing the first cut of this w/AUHAL on OSX so hopefully it's easy to get non-interleaved samples with JACK/ALSA
 */
void ProcessorCore::process(float* in_L, float* in_R, float* out_L, float* out_R, uint32_t n_samp) {
    
    if(master_bypass) {
        memcpy(out_L, in_L, n_samp * sizeof(float));
        memcpy(out_R, in_R, n_samp * sizeof(float));
        return;
    }
    
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

void ProcessorCore::get5bandCompressorGainReduction(float** _bands_gr, float** _bands_lim, bool** _bands_gate_open) {
    proc_mod_5b_compressor->read(_bands_gr, _bands_lim, _bands_gate_open);
}

void ProcessorCore::set_bands_enabled(bool _bands_enabled[]) {
    proc_mod_5b_crossover->band_enable(_bands_enabled[0], _bands_enabled[1], _bands_enabled[2], _bands_enabled[3], _bands_enabled[4]);
}

void ProcessorCore::change_multiband_settings(MULTIBAND_PARAMS _params) {
    proc_mod_5b_compressor->setup(_params);
}


/*  DEFUNCT FACTORY SETTINGS, SHOULD HAPPEN EXTERNAL TO CORE
 COMPRESSOR_PARAMS comp_params_b1 = {
     .drive = -40.0,
     .release = .250,
     .gate_thresh = -50.0,
     .use_coupling = false,
     .coupling = 0.0,
     .window_size = 0.0,
     .window_release = 2.0,
     .ratio = 2.5,
     .idle_gain = 0.0,
     .attack = 0.125,
     .post_gain = 0.0
 };
 
 COMPRESSOR_PARAMS comp_params_b2 = {
     .drive = -40.0,
     .release = 0.240,
     .gate_thresh = -50.0,
     .use_coupling = false,
     .coupling = 0.0,
     .window_size = 0.0,
     .window_release = 2.0,
     .ratio = 1.7,
     .idle_gain = 0.0,
     .attack = 0.080,
     .post_gain = 0.0
 };
 
 COMPRESSOR_PARAMS comp_params_b3 = {
     .drive = -40.0,
     .release = .048,
     .gate_thresh = -50.0,
     .use_coupling = false,
     .coupling = 0.0,
     .window_size = 0.0,
     .window_release = 2.0,
     .ratio = 2,
     .idle_gain = 0.0,
     .attack = 0.016,
     .post_gain = 0.0
 };
 
 COMPRESSOR_PARAMS comp_params_b4 = {
     .drive = -40.0,
     .release = 0.032,
     .gate_thresh = -50.0,
     .use_coupling = true,
     .coupling = 0.20,
     .window_size = 0.0,
     .window_release = 2.0,
     .ratio = 2.2,
     .idle_gain = 0.0,
     .attack = 0.008,
     .post_gain = 0.0
 };
 
 COMPRESSOR_PARAMS comp_params_b5 = {
     .drive = -40.0,
     .release = .016,
     .gate_thresh = -50.0,
     .use_coupling = true,
     .coupling = 0.20,
     .window_size = 0.0,
     .window_release = 2.0,
     .ratio = 2.4,
     .idle_gain = 0.0,
     .attack = 0.004,
     .post_gain = 0.0
 };
 
 /*===================================*/
 /* LIMITERS                          */
 /*===================================*/

/*
 COMPRESSOR_PARAMS lim_params_b1 = {
     .drive = -49.0,
     .release = 0.125,
     .gate_thresh = -100.0,
     .use_coupling = false,
     .coupling = 0.0,
     .window_size = 0.0,
     .window_release = 0.0,
     .ratio = 9,
     .idle_gain = 0.0,
     .attack = 0.100,
     .post_gain = 14.0
 };
 
 COMPRESSOR_PARAMS lim_params_b2 = {
     .drive = -49.0,
     .release = 0.080,
     .gate_thresh = -100.0,
     .use_coupling = false,
     .coupling = 0.0,
     .window_size = 0.0,
     .window_release = 0.0,
     .ratio = 9,
     .idle_gain = 0.0,
     .attack = 0.080,
     .post_gain = 8.0
 };
 
 COMPRESSOR_PARAMS lim_params_b3 = {
     .drive = -49.0,
     .release = 0.016,
     .gate_thresh = -100.0,
     .use_coupling = false,
     .coupling = 0.0,
     .window_size = 0.0,
     .window_release = 0.0,
     .ratio = 9,
     .idle_gain = 0.0,
     .attack = 0.016,
     .post_gain = 8.0
 };
 
 COMPRESSOR_PARAMS lim_params_b4 = {
     .drive = -49.0,
     .release = 0.008,
     .gate_thresh = -100.0,
     .use_coupling = false,
     .coupling = 0.0,
     .window_size = 0.0,
     .window_release = 0.0,
     .ratio = 9,
     .idle_gain = 0.0,
     .attack = 0.008,
     .post_gain = 8.0
 };
 
 COMPRESSOR_PARAMS lim_params_b5 = {
     .drive = -49.0,
     .release = 0.004,
     .gate_thresh = -100.0,
     .use_coupling = false,
     .coupling = 0.0,
     .window_size = 0.0,
     .window_release = 0.0,
     .ratio = 9,
     .idle_gain = 0.0,
     .attack = 0.004,
     .post_gain = 12.0
 };
 
 
 MULTIBAND_PARAMS mb_params = {
     .comp_params[0] = comp_params_b1,
     .lim_params[0] = lim_params_b1,
     .comp_params[1] = comp_params_b2,
     .lim_params[1] = lim_params_b2,
     .comp_params[2] = comp_params_b3,
     .lim_params[2] = lim_params_b3,
     .comp_params[3] = comp_params_b4,
     .lim_params[3] = lim_params_b4,
     .comp_params[4] = comp_params_b5,
     .lim_params[4] = lim_params_b5
 };
 
*/
