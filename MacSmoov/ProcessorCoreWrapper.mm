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
-(id) initWithSampleRate:(uint32_t)sample_rate numberOfChannels:(uint32_t)num_chans bufferSize:(uint32_t)buf_size {
    self = [super init];
#if CORE_BYPASS
#else
    if(self) {
        uint32_t n_samp = buf_size / sizeof(float);
        cpp = new fmsmoov::ProcessorCore(sample_rate, num_chans, n_samp);
        proc_core_hook = &processor_core_hook;
    }
#endif
    return self;
}

-(PROCESSOR_CORE_HOOK) get_proc_core_hook {
    return proc_core_hook;
}

-(BOOL) load_config_from_file:(NSString*) _filename {
    std::string filename([_filename UTF8String]);
#if CORE_BYPASS
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
#if CORE_BYPASS
    /*
     * bypass the core for debugging purposes.  The reason I put this in here is because I have a
     * vexing memory corruption problem, and have no clue where it's happening.
     */
    memcpy(ab_list_coreout->mBuffers[0].mData, ab_list->mBuffers[0].mData, ab_list->mBuffers[0].mDataByteSize);
    memcpy(ab_list_coreout->mBuffers[1].mData, ab_list->mBuffers[1].mData, ab_list->mBuffers[1].mDataByteSize);
    
#else
    cpp->process((float*)ab_list->mBuffers[0].mData, (float*)ab_list->mBuffers[1].mData,
                 (float*)ab_list_coreout->mBuffers[0].mData, (float*)ab_list_coreout->mBuffers[1].mData,
                 ab_list->mBuffers[0].mDataByteSize/sizeof(float));
#endif //CORE_DEBUG
}

-(void) getMainInLevelsLrms:(float*)lrms Rrms:(float*)rrms Lpeak:(float*)lpeak Rpeak:(float*)rpeak {
#if CORE_BYPASS
#else
    cpp->get_main_in_levels(lrms, rrms, lpeak, rpeak);
#endif
}

-(void) setMainInGainDBL:(float)mainInL R:(float)mainInR {
#if CORE_BYPASSS
#else
    cpp->set_main_in_gain_db(mainInL, mainInR);
#endif
}

-(void) get2bandAGCGainReductionlo:(float*)gainReduct2blo hi:(float*)gainReduct2bhi gatelo:(bool*)gate_open_agc2_lo gatehi:(bool*)gate_open_agc2_hi {
#if CORE_BYPASSS
#else
    cpp->get2bandAGCGainReduction(gainReduct2blo, gainReduct2bhi, gate_open_agc2_lo, gate_open_agc2_hi);
#endif
}

-(void) get5bandCompressorGainReduction:(float**) _bands_gr limiters:(float**) _bands_lim gates:(bool**) _bands_gate_open {
#if CORE_BYPASSS
#else
    cpp->get5bandCompressorGainReduction(_bands_gr, _bands_lim, _bands_gate_open);
#endif
}

-(void) setBandEnablement:(NSControlStateValue[]) _bands_enabled {
    bool bands_enabled[5] = {(bool)_bands_enabled[0], (bool)_bands_enabled[1], (bool)_bands_enabled[2], (bool)_bands_enabled[3], (bool)_bands_enabled[4]};
#if CORE_BYPASSS
#else
    cpp->set_bands_enabled(bands_enabled);
#endif
}

-(void) setMasterBypass:(NSControlStateValue) _master_bypass {
#if CORE_BYPASSS
#else
    if(NSControlStateValueOn == _master_bypass) {
        cpp->set_master_bypass(true);
    }
    else {
        cpp->set_master_bypass(false);
    }
#endif
}

-(void) change_multiband_settings:(MULTIBAND_PARAMS)_params {
    cpp->change_multiband_settings(_params);
}

-(void) change_agc_settings:(AGC_PARAMS)_params {
    cpp->change_agc_settings(_params);
}

@end
