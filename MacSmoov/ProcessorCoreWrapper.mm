//
//  ProcessorCoreWrapper.m
//  MacSmoov
//
//  Created by Paul Zaremba on 4/28/23.
//

#import <Foundation/Foundation.h>
#import "ProcessorCoreWrapper.h"
#import "ProcessorCore.hpp"

@interface ProcessorCoreWrapper () {
}
@end

@implementation ProcessorCoreWrapper

fmsmoov::ProcessorCore *cpp;
PROCESSOR_CORE_HOOK proc_core_hook;

//This is a non-interleaved buffer, so the total number of samples is twice what the buf size is
-(id) initWithSampleRate:(uint32_t)sample_rate numberOfChannels:(uint32_t)num_chans bufferSize:(uint32_t)buf_size configFilename:(NSString *)filename {
    self = [super init];
//#if CORE_BYPASS
//#else
    if(self) {
        uint32_t n_samp = buf_size / sizeof(float);
        cpp = new fmsmoov::ProcessorCore(sample_rate, num_chans, n_samp, std::string([filename UTF8String]));
        proc_core_hook = &processor_core_hook;
    }
//#endif
    return self;
}

-(PROCESSOR_CORE_HOOK) get_proc_core_hook {
    return proc_core_hook;
}

-(BOOL) load_config_from_file:(NSString*) _filename {
    std::string filename([_filename UTF8String]);
#if false
    return YES;
#else
    return cpp->load_config_from_file(filename);
#endif
}

-(BOOL) prepare {
#if CORE_BYPASS
    return YES;
#else
    return cpp->prepare();
#endif
}

void processor_core_hook(AudioBufferList* ab_list, AudioBufferList* ab_list_coreout) {
#if false
    /*
     * bypass the core for debugging purposes.  The reason I put this in here is because I have a
     * vexing memory corruption problem, and have no clue where it's happening.
     */
    memcpy(ab_list_coreout->mBuffers[0].mData, ab_list->mBuffers[0].mData, ab_list->mBuffers[0].mDataByteSize);
    memcpy(ab_list_coreout->mBuffers[1].mData, ab_list->mBuffers[1].mData, ab_list->mBuffers[1].mDataByteSize);
    
#else
    try {
        cpp->process((float*)ab_list->mBuffers[0].mData, (float*)ab_list->mBuffers[1].mData,
                     (float*)ab_list_coreout->mBuffers[0].mData, (float*)ab_list_coreout->mBuffers[1].mData,
                     ab_list->mBuffers[0].mDataByteSize/sizeof(float));
    } catch (const std::exception& ex) {
        NSLog(@"Caught a C++ exception in the processor core: %s", ex.what());
        [NSException raise:@"PROCESSOR CORE EXCEPTION" format:@"%s", ex.what()];
    }
#endif //CORE_BYPASS
}

-(void) getMainInLevelsLrms:(float*)lrms Rrms:(float*)rrms Lpeak:(float*)lpeak Rpeak:(float*)rpeak {
#if false
#else
    cpp->get_main_in_levels(lrms, rrms, lpeak, rpeak);
#endif
}

-(void) getMainOutLevelsLrms:(float*)lrms Rrms:(float*)rrms Lpeak:(float*)lpeak Rpeak:(float*)rpeak {
    cpp->get_main_out_levels(lrms, rrms, lpeak, rpeak);
}


-(void) setMainInGainDBL:(float)mainInL R:(float)mainInR {
#if CORE_BYPASS
#else
    cpp->set_main_in_gain_db(mainInL, mainInR);
#endif
}

-(void) mainInGainChangeDoneL:(float)mainInL R:(float)mainInR { /* Only fired when the mouse button is lifted or a key is pressed on the control to minimize cfg file writes */
    cpp->main_in_gain_db_change_done(mainInL, mainInR);
}

-(void) getMainInGainDBL:(float*)mainInL R:(float*)mainInR {
    cpp->get_main_in_gain_db(mainInL, mainInR);
}

-(bool) getStereoEnhanceEnabled {
    return cpp->get_stereo_enhance_enabled();
}

-(void) setStereoEnhanceEnabled:(NSControlStateValue) _se_enabled {
    if(NSControlStateValueOn == _se_enabled) {
        cpp->set_stereo_enhance_enabled(true);
    }
    else {
        cpp->set_stereo_enhance_enabled(false);
    }
}

-(void) getStereoEnhanceLRDiff:(float*)_lr_diff {
    cpp->get_stereo_enhance_lr_diff(_lr_diff);
}

-(float) getStereoEnhanceDrive {
    return cpp->get_stereo_enhance_drive();
}

-(void) setStereoEnhanceDrive:(float)drive {
    cpp->set_stereo_enhance_drive(drive);
}

-(void) get2bandAGCGainReductionlo:(float*)gainReduct2blo hi:(float*)gainReduct2bhi gatelo:(bool*)gate_open_agc2_lo gatehi:(bool*)gate_open_agc2_hi {
#if CORE_BYPASS
#else
    cpp->get2bandAGCGainReduction(gainReduct2blo, gainReduct2bhi, gate_open_agc2_lo, gate_open_agc2_hi);
#endif
}

-(void) get5bandCompressorGainReduction:(float**) _bands_gr limiters:(float**) _bands_lim gates:(bool**) _bands_gate_open {
#if CORE_BYPASS
#else
    cpp->get5bandCompressorGainReduction(_bands_gr, _bands_lim, _bands_gate_open);
#endif
}

-(void) getLookaheadLimiterGainReduction:(float*)_lookahead_limiter_gr {
    cpp->getLookaheadLimiterGainReduction(_lookahead_limiter_gr);
}

-(void) getClipperActionL:(float*)_action_l R:(float*)_action_r {
    cpp->getClipperAction(_action_l, _action_r);
}

-(void) setBandEnablement:(NSControlStateValue[]) _bands_enabled {
    bool bands_enabled[5] = {(bool)_bands_enabled[0], (bool)_bands_enabled[1], (bool)_bands_enabled[2], (bool)_bands_enabled[3], (bool)_bands_enabled[4]};
#if CORE_BYPASS
#else
    cpp->set_bands_enabled(bands_enabled);
#endif
}

-(void) setMasterBypass:(NSControlStateValue) _master_bypass {
#if CORE_BYPASS
#else
    if(NSControlStateValueOn == _master_bypass) {
        cpp->set_master_bypass(true);
    }
    else {
        cpp->set_master_bypass(false);
    }
#endif
}

-(void) get_agc_settings:(AGC_PARAMS*)_params {
    cpp->get_agc_settings(*_params);
}

-(void) change_agc_settings:(AGC_PARAMS)_params {
#if CORE_BYPASS
#else
    cpp->change_agc_settings(_params);
#endif
}

-(void) get_multiband_settings:(MULTIBAND_PARAMS*)_params {
    MULTIBAND_PARAMS core_params;
    cpp->get_multiband_settings(core_params);
    *_params = core_params;
}

-(void) change_multiband_settings:(MULTIBAND_PARAMS)_params {
#if CORE_BYPASS
#else
    cpp->change_multiband_settings(_params);
#endif
}

-(float) get_clip_level {
    return cpp->get_clip_level();
}

-(void) set_clip_level:(float)_clip_level {
    cpp->set_clip_level(_clip_level);
}

-(void) setMainOutGainDBL:(float)mainInL R:(float)mainInR {
    cpp->set_main_out_gain_db(mainInL, mainInR);
}

-(void) mainOutGainChangeDoneL:(float)mainInL R:(float)mainInR { /* Only fired when the mouse button is lifted or a key is pressed on the control to minimize cfg file writes */
    cpp->main_out_gain_db_change_done(mainInL, mainInR);
}

-(void) getMainOutGainDBL:(float*)mainOutL R:(float*)mainOutR {
    cpp->get_main_out_gain_db(mainOutL, mainOutR);
}


@end
