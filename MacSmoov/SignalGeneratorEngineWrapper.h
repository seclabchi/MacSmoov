//
//  SignalGeneratorEngineWrapper.h
//  MacSmoov
//
//  Created by Paul Zaremba on 4/2/23.
//

#ifndef SignalGeneratorEngineWrapper_h
#define SignalGeneratorEngineWrapper_h

#import "CommonTypes.h"

@interface SignalGeneratorEngineWrapper : NSObject
-(id) initWithSampleRate:(uint32_t)sample_rate numberOfChannels:(uint32_t)num_chans;
-(void) configureWithType:(SIG_GEN_TYPE) type frequency:(uint32_t)frequency;
-(void) getNextBuffer:(float*) out_buf ofSize:(uint32_t) n_frames;
-(void) adjustVolume:(float) volume;
@end


#endif /* SignalGeneratorEngineWrapper_h */
