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
ProcessorCore::ProcessorCore(uint32_t _f_samp, uint32_t _n_channels, uint32_t _n_samp, const std::string& _config_filename) : f_samp(_f_samp), n_channels(_n_channels), n_samp(_n_samp) {
    core_config = CoreConfig::get_instance();
    core_stack = CoreStack::getInstance();
    
    m_loglin = new LogLinConverter(LogLinConversionType::LOG_TO_LIN);
    m_linlog = new LogLinConverter(LogLinConversionType::LIN_TO_LOG);
    
    core_config->load_cfg_from_file(_config_filename);

    master_bypass = false;
    
    proc_mod_gain_main_in = new ProcModGain("GAIN_MAIN_IN", f_samp, n_channels, n_samp);
    proc_mod_gain_main_in->init(core_config, nullptr, nullptr);
    core_stack->add_module(proc_mod_gain_main_in);
    
    ChannelMap* chan_map_pm_ihpf = new ChannelMap();
    chan_map_pm_ihpf->the_map.push_back(CHANNEL_MAP_ELEMENT {0, 0, "IN_L"});
    chan_map_pm_ihpf->the_map.push_back(CHANNEL_MAP_ELEMENT {1, 1, "IN_R"});
    proc_mod_input_hpf = new ProcModInputHPF("INPUT_HPF", f_samp, n_channels, n_samp);
    proc_mod_input_hpf->init(core_config, proc_mod_gain_main_in, chan_map_pm_ihpf);
    core_stack->add_module(proc_mod_input_hpf);
    
    ChannelMap* chan_map_pm_lmi = new ChannelMap();
    chan_map_pm_lmi->the_map.push_back(CHANNEL_MAP_ELEMENT {0, 0, "IN_L"});
    chan_map_pm_lmi->the_map.push_back(CHANNEL_MAP_ELEMENT {1, 1, "IN_R"});
    proc_mod_level_main_in = new ProcModLevelMeter("LEVEL_MAIN_IN", f_samp, n_channels, n_samp);
    proc_mod_level_main_in->init(core_config, proc_mod_input_hpf, chan_map_pm_lmi);
    core_stack->add_module(proc_mod_level_main_in);
    
    ChannelMap* chan_map_pm_se = new ChannelMap();
    chan_map_pm_se->the_map.push_back(CHANNEL_MAP_ELEMENT {0, 0, "IN_L"});
    chan_map_pm_se->the_map.push_back(CHANNEL_MAP_ELEMENT {1, 1, "IN_R"});
    proc_mod_stereo_enhance = new ProcModStereoEnhance("STEREO_ENHANCE", f_samp, n_channels, n_samp);
    proc_mod_stereo_enhance->init(core_config, proc_mod_level_main_in, chan_map_pm_se);
    core_stack->add_module(proc_mod_stereo_enhance);
    
    ChannelMap* chan_map_pm_2ba = new ChannelMap();
    chan_map_pm_2ba->the_map.push_back(CHANNEL_MAP_ELEMENT {0, 0, "IN_L"});
    chan_map_pm_2ba->the_map.push_back(CHANNEL_MAP_ELEMENT {1, 1, "IN_R"});
    proc_mod_2band_agc = new ProcMod2BandAGC("2BAND_AGC", f_samp, n_channels, n_samp);
    proc_mod_2band_agc->init(core_config, proc_mod_stereo_enhance, chan_map_pm_2ba);
    core_stack->add_module(proc_mod_2band_agc);

    ChannelMap* chan_map_he = new ChannelMap();
    chan_map_he->the_map.push_back(CHANNEL_MAP_ELEMENT {0, 0, "IN_L"});
    chan_map_he->the_map.push_back(CHANNEL_MAP_ELEMENT {1, 1, "IN_R"});
    proc_mod_hf_enhance = new ProcModHFEnhance("HF_ENHANCE", f_samp, n_channels, n_samp);
    proc_mod_hf_enhance->init(core_config, proc_mod_2band_agc, chan_map_he);
    core_stack->add_module(proc_mod_hf_enhance);
    
    ChannelMap* chan_map_5bcross = new ChannelMap();
    chan_map_5bcross->the_map.push_back(CHANNEL_MAP_ELEMENT {0, 0, "IN_L"});
    chan_map_5bcross->the_map.push_back(CHANNEL_MAP_ELEMENT {1, 1, "IN_R"});
    proc_mod_5b_crossover = new ProcMod5bandCrossover("5BAND_CROSSOVER", f_samp, n_channels, n_samp);
    proc_mod_5b_crossover->init(core_config, proc_mod_hf_enhance, chan_map_5bcross);
    core_stack->add_module(proc_mod_5b_crossover);
    
    ChannelMap* chan_map_5bcomp = new ChannelMap();
    chan_map_5bcomp->the_map.push_back(CHANNEL_MAP_ELEMENT {0, 0, "IN_L"});
    chan_map_5bcomp->the_map.push_back(CHANNEL_MAP_ELEMENT {1, 1, "IN_R"});
    chan_map_5bcomp->the_map.push_back(CHANNEL_MAP_ELEMENT {2, 2, "IN_B1_L"});
    chan_map_5bcomp->the_map.push_back(CHANNEL_MAP_ELEMENT {3, 3, "IN_B1_R"});
    chan_map_5bcomp->the_map.push_back(CHANNEL_MAP_ELEMENT {4, 4, "IN_B2_L"});
    chan_map_5bcomp->the_map.push_back(CHANNEL_MAP_ELEMENT {5, 5, "IN_B2_R"});
    chan_map_5bcomp->the_map.push_back(CHANNEL_MAP_ELEMENT {6, 6, "IN_B3_L"});
    chan_map_5bcomp->the_map.push_back(CHANNEL_MAP_ELEMENT {7, 7, "IN_B3_R"});
    chan_map_5bcomp->the_map.push_back(CHANNEL_MAP_ELEMENT {8, 8, "IN_B4_L"});
    chan_map_5bcomp->the_map.push_back(CHANNEL_MAP_ELEMENT {9, 9, "IN_B4_R"});
    chan_map_5bcomp->the_map.push_back(CHANNEL_MAP_ELEMENT {10, 10, "IN_B5_L"});
    chan_map_5bcomp->the_map.push_back(CHANNEL_MAP_ELEMENT {11, 11, "IN_B5_R"});
    proc_mod_5b_compressor = new ProcMod5bandCompressor("5BAND_COMPRESSOR", f_samp, n_channels, n_samp);
    proc_mod_5b_compressor->init(core_config, proc_mod_5b_crossover, chan_map_5bcomp);
    core_stack->add_module(proc_mod_5b_compressor);
    
    ChannelMap* chan_map_ll = new ChannelMap();
    chan_map_ll->the_map.push_back(CHANNEL_MAP_ELEMENT {0, 0, "IN_L"});
    chan_map_ll->the_map.push_back(CHANNEL_MAP_ELEMENT {1, 1, "IN_R"});
    proc_mod_lookahead_limiter = new ProcModLookaheadLimiter("LOOKAHEAD_LIMITER", f_samp, n_channels, n_samp);
    proc_mod_lookahead_limiter->init(core_config, proc_mod_5b_compressor, chan_map_ll);
    core_stack->add_module(proc_mod_lookahead_limiter);
    
    ChannelMap* chan_map = new ChannelMap();
    chan_map->the_map.push_back(CHANNEL_MAP_ELEMENT {0, 0, "IN_L"});
    chan_map->the_map.push_back(CHANNEL_MAP_ELEMENT {1, 1, "IN_R"});
    proc_mod_clipper = new ProcModClipper("CLIPPER", f_samp, n_channels, n_samp);
    proc_mod_clipper->init(core_config, proc_mod_lookahead_limiter, chan_map);
    core_stack->add_module(proc_mod_clipper);
    
    chan_map = new ChannelMap();
    chan_map->the_map.push_back(CHANNEL_MAP_ELEMENT {0, 0, "IN_L"});
    chan_map->the_map.push_back(CHANNEL_MAP_ELEMENT {1, 1, "IN_R"});
    proc_mod_gain_main_out = new ProcModGain("GAIN_MAIN_OUT", f_samp, n_channels, n_samp);
    proc_mod_gain_main_out->init(core_config, proc_mod_clipper, chan_map);
    core_stack->add_module(proc_mod_gain_main_out);
    
    chan_map = new ChannelMap();
    chan_map->the_map.push_back(CHANNEL_MAP_ELEMENT {0, 0, "IN_L"});
    chan_map->the_map.push_back(CHANNEL_MAP_ELEMENT {1, 1, "IN_R"});
    proc_mod_final_lpf = new ProcModFinalLPF("FINAL_LPF", f_samp, n_channels, n_samp);
    proc_mod_final_lpf->init(core_config, proc_mod_gain_main_out, chan_map);
    core_stack->add_module(proc_mod_final_lpf);
    
    chan_map = new ChannelMap();
    chan_map->the_map.push_back(CHANNEL_MAP_ELEMENT {0, 0, "IN_L"});
    chan_map->the_map.push_back(CHANNEL_MAP_ELEMENT {1, 1, "IN_R"});
    proc_mod_level_main_out = new ProcModLevelMeter("LEVEL_MAIN_OUT", f_samp, n_channels, n_samp);
    proc_mod_level_main_out->init(core_config, proc_mod_final_lpf, chan_map);
    core_stack->add_module(proc_mod_level_main_out);
}

bool ProcessorCore::prepare() {
    return core_stack->is_ready();
}

bool ProcessorCore::load_config_from_file(const std::string& filename) {
    bool retval = false;
    AGC_PARAMS agc_params;
    MULTIBAND_PARAMS mb_params;
    
    retval = core_config->load_cfg_from_file(filename);
    
    if(retval) {
        core_config->get_agc_params(agc_params);
        proc_mod_stereo_enhance->set_bypass(!(core_config->get_stereo_enhance_enabled()));
        proc_mod_2band_agc->configure(agc_params);
        proc_mod_5b_crossover->set_bypass(!(core_config->get_mb_crossover_enabled()));
        core_config->get_mb_params(mb_params);
        proc_mod_5b_compressor->configure(mb_params);
        proc_mod_5b_compressor->set_bypass(!(core_config->get_mb_compressor_enabled()));
    }
    
    return retval;
}

bool ProcessorCore::write_config_changes_agc(const AGC_PARAMS& _params) {
    return core_config->set_agc_params(_params);
}

bool ProcessorCore::write_config_changes_multiband(const MULTIBAND_PARAMS& _params) {
    return core_config->set_mb_params(_params);
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
    
    core_stack->process(in_L, in_R, out_L, out_R, n_samp);
    
}

void ProcessorCore::get_main_in_levels(float* lrms, float* rrms, float* lpeak, float* rpeak) {
    proc_mod_level_main_in->get_levels_db(lrms, rrms, lpeak, rpeak);
}

void ProcessorCore::get_main_out_levels(float* lrms, float* rrms, float* lpeak, float* rpeak) {
    proc_mod_level_main_out->get_levels_db(lrms, rrms, lpeak, rpeak);
}

void ProcessorCore::set_main_in_gain_db(float loggain_l, float loggain_r) {
    proc_mod_gain_main_in->set_gain_db(loggain_l, loggain_r);
}

void ProcessorCore::main_in_gain_db_change_done(float loggain_l, float loggain_r) {
    core_config->set_input_gain(std::pair<float,float>(loggain_l, loggain_r));
}

void ProcessorCore::get_main_in_gain_db(float* gain_in_db_L, float* gain_in_db_R) {
    std::pair<float, float> input_gain_db;
    core_config->get_input_gain(input_gain_db);
    *gain_in_db_L = input_gain_db.first;
    *gain_in_db_R = input_gain_db.second;
}

void ProcessorCore::set_stereo_enhance_enabled(bool enabled) {
    proc_mod_stereo_enhance->set_bypass(!enabled);
    core_config->set_stereo_enhance_enabled(enabled);
}

bool ProcessorCore::get_stereo_enhance_enabled() {
    return core_config->get_stereo_enhance_enabled();
}

void ProcessorCore::get_stereo_enhance_lr_diff(float* _lr_diff) {
    proc_mod_stereo_enhance->read(_lr_diff);
}

float ProcessorCore::get_stereo_enhance_drive() {
    return core_config->get_stereo_enhance_drive();
}

void ProcessorCore::set_stereo_enhance_drive(float _drive) {
    proc_mod_stereo_enhance->configure();
    core_config->set_stereo_enhance_drive(_drive);
}

void ProcessorCore::get2bandAGCGainReduction(float* gainReduct2blo, float* gainReduct2bhi, bool* gateOpenLo, bool* gateOpenHi) {
    proc_mod_2band_agc->read(gainReduct2blo, gainReduct2bhi, gateOpenLo, gateOpenHi);
    //cout << "GainredL=" << *gainReduct2blo << " H=" << *gainReduct2bhi << " GATES " << gate_open_lo << "|" << gate_open_hi << endl;
}

void ProcessorCore::get5bandCompressorGainReduction(float** _bands_gr, float** _bands_lim, bool** _bands_gate_open) {
    proc_mod_5b_compressor->read(_bands_gr, _bands_lim, _bands_gate_open);
}

void ProcessorCore::getLookaheadLimiterGainReduction(float* _lookahead_gr) {
    proc_mod_lookahead_limiter->read(_lookahead_gr);
}

void ProcessorCore::getClipperAction(float* _action_l, float* _action_r) {
    proc_mod_clipper->read(_action_l, _action_r);
}

void ProcessorCore::set_bands_enabled(bool _bands_enabled[]) {
    proc_mod_5b_crossover->band_enable(_bands_enabled[0], _bands_enabled[1], _bands_enabled[2], _bands_enabled[3], _bands_enabled[4]);
}

void ProcessorCore::set_master_bypass(bool _master_bypass) {
    master_bypass = _master_bypass;
}

void ProcessorCore::get_agc_settings(AGC_PARAMS& _params) {
    core_config->get_agc_params(_params);
}

bool ProcessorCore::change_agc_settings(const AGC_PARAMS& _params) {
    proc_mod_2band_agc->configure(_params);
    write_config_changes_agc(_params);
    return true;
}

void ProcessorCore::get_multiband_settings(MULTIBAND_PARAMS& _params) {
    core_config->get_mb_params(_params);
}

bool ProcessorCore::change_multiband_settings(const MULTIBAND_PARAMS& _params) {
    
    /*
     * This is HOKEY as fuck.  Need to figure out a better way to manipulate
     * common config values across the bands.
     */
    
    MULTIBAND_PARAMS mbp = _params;
    mbp.comp_params[0].gate_thresh = _params.gate_thresh;
    mbp.comp_params[1].gate_thresh = _params.gate_thresh;
    mbp.comp_params[2].gate_thresh = _params.gate_thresh;
    mbp.comp_params[3].gate_thresh = _params.gate_thresh;
    mbp.comp_params[4].gate_thresh = _params.gate_thresh;
    
    proc_mod_5b_compressor->configure(mbp);
    proc_mod_5b_crossover->configure(mbp.drive);
    write_config_changes_multiband(mbp);
    
    return true;
}

float ProcessorCore::get_clip_level() {
    return core_config->get_clip_level();
}

void ProcessorCore::set_clip_level(float _clip_level) {
    proc_mod_clipper->configure(_clip_level);
    core_config->set_clip_level(_clip_level);
}

void ProcessorCore::set_main_out_gain_db(float loggain_l, float loggain_r) {
    proc_mod_gain_main_out->set_gain_db(loggain_l, loggain_r);
}

void ProcessorCore::main_out_gain_db_change_done(float loggain_l, float loggain_r) {
    core_config->set_output_gain(std::pair<float,float>(loggain_l, loggain_r));
}

void ProcessorCore::get_main_out_gain_db(float* gain_out_db_L, float* gain_out_db_R) {
    std::pair<float, float> output_gain_db;
    core_config->get_output_gain(output_gain_db);
    *gain_out_db_L = output_gain_db.first;
    *gain_out_db_R = output_gain_db.second;
}
