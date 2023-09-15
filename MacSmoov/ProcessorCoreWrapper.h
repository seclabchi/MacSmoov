//
//  ProcessorCoreWrapper.h
//  MacSmoov
//
//  Created by Paul Zaremba on 4/28/23.
//

#ifndef ProcessorCoreWrapper_h
#define ProcessorCoreWrapper_h

#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#include "core/core_common.h"

typedef void(*PROCESSOR_CORE_HOOK)(AudioBufferList* ab_list, AudioBufferList* ab_list_coreout);

@interface ProcessorCoreWrapper : NSObject
-(id) initWithSampleRate:(uint32_t)sample_rate numberOfChannels:(uint32_t)num_chans bufferSize:(uint32_t)buf_size;
-(PROCESSOR_CORE_HOOK) get_proc_core_hook;
//-(void) processWithInput:(float*)in_buf output:(float*)out_buf ofSize:(uint32_t) n_samp;
-(void) getMainInLevelsLrms:(float*)lrms Rrms:(float*)rrms Lpeak:(float*)lpeak Rpeak:(float*)rpeak;
-(void) setMainInGainDBL:(float)mainInL R:(float)mainInR;
-(void) get2bandAGCGainReductionlo:(float*)gainReduct2blo hi:(float*)gainReduct2bhi gatelo:(bool*)gate_open_agc2_lo gatehi:(bool*)gate_open_agc2_hi;
-(void) get5bandCompressorGainReduction:(float**) _bands_gr limiters:(float**) _bands_lim gates:(bool**) _bands_gate_open;
-(void) setBandEnablement:(NSControlStateValue[]) _bands_enabled;
-(void) change_multiband_settings:(MULTIBAND_PARAMS)_params;

@end

void processor_core_hook(AudioBufferList* ab_list, AudioBufferList* ab_list_coreout);

#endif /* ProcessorCoreWrapper_h */
