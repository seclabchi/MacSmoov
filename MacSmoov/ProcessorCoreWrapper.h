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
-(id) initWithSampleRate:(uint32_t)sample_rate numberOfChannels:(uint32_t)num_chans bufferSize:(uint32_t)buf_size configFilename:(NSString*)filename;
-(PROCESSOR_CORE_HOOK) get_proc_core_hook;
-(BOOL) load_config_from_file:(NSString*) _filename;
-(BOOL) prepare;
//-(void) processWithInput:(float*)in_buf output:(float*)out_buf ofSize:(uint32_t) n_samp;
-(void) getMainInLevelsLrms:(float*)lrms Rrms:(float*)rrms Lpeak:(float*)lpeak Rpeak:(float*)rpeak;
-(void) getMainOutLevelsLrms:(float*)lrms Rrms:(float*)rrms Lpeak:(float*)lpeak Rpeak:(float*)rpeak;
-(void) getLookaheadLimiterGainReduction:(float*)_lookahead_limiter_gr;
-(void) getClipperActionL:(float*)_action_l R:(float*)_action_r;
-(void) setMainInGainDBL:(float)mainInL R:(float)mainInR;
-(void) mainInGainChangeDoneL:(float)mainInL R:(float)mainInR; /* Only fired when the mouse button is lifted or a key is pressed on the control to minimize cfg file writes */
-(void) getMainInGainDBL:(float*)mainInL R:(float*)mainInR;
-(bool) getStereoEnhanceEnabled;
-(void) setStereoEnhanceEnabled:(NSControlStateValue) _se_enabled;
-(void) getStereoEnhanceLRDiff:(float*)_lr_diff;
-(float) getStereoEnhanceDrive;
-(void) setStereoEnhanceDrive:(float)drive;
-(void) get2bandAGCGainReductionlo:(float*)gainReduct2blo hi:(float*)gainReduct2bhi gatelo:(bool*)gate_open_agc2_lo gatehi:(bool*)gate_open_agc2_hi;
-(void) get5bandCompressorGainReduction:(float**) _bands_gr limiters:(float**) _bands_lim gates:(bool**) _bands_gate_open;
-(void) setBandEnablement:(NSControlStateValue[]) _bands_enabled;
-(void) setMasterBypass:(NSControlStateValue) _master_bypass;
-(void) get_agc_settings:(AGC_PARAMS*)_params;
-(void) change_agc_settings:(AGC_PARAMS)_params;
-(void) get_multiband_settings:(MULTIBAND_PARAMS*)_params;
-(void) change_multiband_settings:(MULTIBAND_PARAMS)_params;
-(float) get_clip_level;
-(void) set_clip_level:(float)_clip_level;
-(void) setMainOutGainDBL:(float)mainInL R:(float)mainInR;
-(void) mainOutGainChangeDoneL:(float)mainInL R:(float)mainInR; /* Only fired when the mouse button is lifted or a key is pressed on the control to minimize cfg file writes */
-(void) getMainOutGainDBL:(float*)mainOutL R:(float*)mainOutR;


@end

void processor_core_hook(AudioBufferList* ab_list, AudioBufferList* ab_list_coreout);

#endif /* ProcessorCoreWrapper_h */
