//
//  ProcessorCore.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 4/15/23.
//

#ifndef ProcessorCore_hpp
#define ProcessorCore_hpp

#include <stdio.h>
#include <cstdint>

#include "CoreConfig.h"
#include "CoreStack.hpp"
#include "core_utils/channel_map.hpp"
#include "LogLinConverter.hpp"

#include "ProcModInputHPF.hpp"
#include "ProcModLevelMeter.hpp"
#include "ProcModGain.hpp"
#include "ProcModStereoEnhance.hpp"
#include "ProcMod2BandAGC.hpp"
#include "ProcModHFEnhance.hpp"
#include "ProcMod5bandCrossover.hpp"
#include "ProcMod5bandCompressor.hpp"
#include "ProcModLookaheadLimiter.h"
#include "ProcModClipper.hpp"
#include "ProcModFinalLPF.hpp"


/*
 * Define this to enable core debugging.  Right now it just bypasses the core entirely, so not
 * sure about putting this here.
 */
#define CORE_BYPASS false
#define CORE_DEBUG true

namespace fmsmoov {

using namespace std;

class ProcessorCore {
public:
    ProcessorCore(uint32_t _f_samp, uint32_t _n_channels, uint32_t _n_samp, const std::string& _config_filename);
    bool load_config_from_file(const std::string& filename);
    bool prepare();
    void process(float* in_L, float* in_R, float* out_L, float* out_R, uint32_t n_samp);
    void get_main_in_levels(float* lrms, float* rrms, float* lpeak, float* rpeak);
    void get_main_out_levels(float* lrms, float* rrms, float* lpeak, float* rpeak);
    void set_main_in_gain_db(float loggain_l, float loggain_r);
    void get_main_in_gain_db(float* gain_in_db_L, float* gain_in_db_R);
    void main_in_gain_db_change_done(float loggain_l, float loggain_r);
    void set_stereo_enhance_enabled(bool enabled);
    bool get_stereo_enhance_enabled();
    void get_stereo_enhance_lr_diff(float* _lr_diff);
    float get_stereo_enhance_drive();
    void set_stereo_enhance_drive(float _drive);
    void get2bandAGCGainReduction(float* gainReduct2blo, float* gainReduct2bhi, bool* gateOpenLo, bool* gateOpenHi);
    void get5bandCompressorGainReduction(float** _bands_gr, float** _bands_lim, bool** _bands_gate_open);
    void getLookaheadLimiterGainReduction(float* _lookahead_gr);
    void getClipperAction(float* _action_l, float* _action_r);
    void set_bands_enabled(bool _bands_enabled[]);
    void set_master_bypass(bool _master_bypass);
    void get_agc_settings(AGC_PARAMS& _params);
    bool change_agc_settings(const AGC_PARAMS& _params);
    void get_multiband_settings(MULTIBAND_PARAMS& _params);
    bool change_multiband_settings(const MULTIBAND_PARAMS& _params);
    float get_clip_level();
    void set_clip_level(float _clip_level);
    void set_main_out_gain_db(float loggain_l, float loggain_r);
    void main_out_gain_db_change_done(float loggain_l, float loggain_r);
    void get_main_out_gain_db(float* gain_out_db_L, float* gain_out_db_R);
private:
    ProcessorCore();
    bool write_config_changes_agc(const AGC_PARAMS& _params);
    bool write_config_changes_multiband(const MULTIBAND_PARAMS& _params);

    float f_samp;
    uint32_t n_channels;
    uint32_t n_samp;  //total interleaved L+R samples
    bool master_bypass;
    
    CoreConfig* core_config;
    CoreStack* core_stack;
    
    ProcModInputHPF* proc_mod_input_hpf;
    ProcModGain* proc_mod_gain_main_in;
    ProcModLevelMeter* proc_mod_level_main_in;
    ProcModStereoEnhance* proc_mod_stereo_enhance;
    ProcMod2BandAGC* proc_mod_2band_agc;
    ProcModHFEnhance* proc_mod_hf_enhance;
    ProcMod5bandCrossover* proc_mod_5b_crossover;
    ProcMod5bandCompressor* proc_mod_5b_compressor;
    ProcModLookaheadLimiter* proc_mod_lookahead_limiter;
    ProcModClipper* proc_mod_clipper;
    ProcModGain* proc_mod_gain_main_out;
    ProcModFinalLPF* proc_mod_final_lpf;
    ProcModLevelMeter* proc_mod_level_main_out;

    LogLinConverter* m_loglin;
    LogLinConverter* m_linlog;
    
};

}
#endif /* ProcessorCore_hpp */
