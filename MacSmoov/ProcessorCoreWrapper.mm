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
    NSUserDefaults* prefs;
}
@end

@implementation ProcessorCoreWrapper

fmsmoov::ProcessorCore *cpp;
PROCESSOR_CORE_HOOK proc_core_hook;

//This is a non-interleaved buffer, so the total number of samples is twice what the buf size is
-(id) initWithSampleRate:(uint32_t)sample_rate numberOfChannels:(uint32_t)num_chans bufferSize:(uint32_t)buf_size
            defaults:(NSUserDefaults*) defaults {
    self = [super init];
    if(self) {
        uint32_t n_samp = buf_size / sizeof(float);
        cpp = new fmsmoov::ProcessorCore(sample_rate, num_chans, n_samp);
        proc_core_hook = &processor_core_hook;
        prefs = defaults;
    }
    
    return self;
}

-(PROCESSOR_CORE_HOOK) get_proc_core_hook {
    return proc_core_hook;
}

void processor_core_hook(AudioBufferList* ab_list, AudioBufferList* ab_list_coreout) {
    cpp->process((float*)ab_list->mBuffers[0].mData, (float*)ab_list->mBuffers[1].mData,
                 (float*)ab_list_coreout->mBuffers[0].mData, (float*)ab_list_coreout->mBuffers[1].mData,
                 ab_list->mBuffers[0].mDataByteSize/sizeof(float));
}

-(void) getMainInLevelsLrms:(float*)lrms Rrms:(float*)rrms Lpeak:(float*)lpeak Rpeak:(float*)rpeak {
    cpp->get_main_in_levels(lrms, rrms, lpeak, rpeak);
}

-(void) setMainInGainDBL:(float)mainInL R:(float)mainInR {
    cpp->set_main_in_gain_db(mainInL, mainInR);
}

-(void) get2bandAGCGainReductionlo:(float*)gainReduct2blo hi:(float*)gainReduct2bhi gatelo:(bool*)gate_open_agc2_lo gatehi:(bool*)gate_open_agc2_hi {
    cpp->get2bandAGCGainReduction(gainReduct2blo, gainReduct2bhi, gate_open_agc2_lo, gate_open_agc2_hi);
}

-(void) get5bandCompressorGainReduction:(float**) _bands_gr limiters:(float**) _bands_lim gates:(bool**) _bands_gate_open {
    cpp->get5bandCompressorGainReduction(_bands_gr, _bands_lim, _bands_gate_open);
}

-(void) setBandEnablement:(NSControlStateValue[]) _bands_enabled {
    bool bands_enabled[5] = {(bool)_bands_enabled[0], (bool)_bands_enabled[1], (bool)_bands_enabled[2], (bool)_bands_enabled[3], (bool)_bands_enabled[4]};
    cpp->set_bands_enabled(bands_enabled);
    [prefs setBool:_bands_enabled[0] forKey:@"ENABLE_B1"];
    [prefs setBool:_bands_enabled[1] forKey:@"ENABLE_B2"];
    [prefs setBool:_bands_enabled[2] forKey:@"ENABLE_B3"];
    [prefs setBool:_bands_enabled[3] forKey:@"ENABLE_B4"];
    [prefs setBool:_bands_enabled[4] forKey:@"ENABLE_B5"];
}

-(void) change_multiband_settings:(MULTIBAND_PARAMS)_params {
    cpp->change_multiband_settings(_params);
}

-(void) read_prefs {
    NSControlStateValue be[5];
    be[0] = [prefs boolForKey:@"ENABLE_B1"];
    be[1] = [prefs boolForKey:@"ENABLE_B2"];
    be[2] = [prefs boolForKey:@"ENABLE_B3"];
    be[3] = [prefs boolForKey:@"ENABLE_B4"];
    be[4] = [prefs boolForKey:@"ENABLE_B5"];
    
    [self setBandEnablement:be];
    
}

@end
