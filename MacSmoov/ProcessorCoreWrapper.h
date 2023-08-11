//
//  ProcessorCoreWrapper.h
//  MacSmoov
//
//  Created by Paul Zaremba on 4/28/23.
//

#ifndef ProcessorCoreWrapper_h
#define ProcessorCoreWrapper_h

@interface ProcessorCoreWrapper : NSObject
-(id) initWithSampleRate:(uint32_t)sample_rate numberOfChannels:(uint32_t)num_chans bufSize:(uint32_t)buf_size;
-(void) processWithInput:(float*)in_buf output:(float*)out_buf ofSize:(uint32_t) n_frames;
-(void) getMainInLevelsLrms:(float*)lrms Rrms:(float*)rrms Lpeak:(float*)lpeak Rpeak:(float*)rpeak;
-(void) setMainInGainDBL:(float)mainInL R:(float)mainInR;

@end

#endif /* ProcessorCoreWrapper_h */
