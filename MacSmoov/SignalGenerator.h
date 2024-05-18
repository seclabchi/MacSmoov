//
//  SignalGenerator.h
//  MacSmoov
//
//  Created by Paul Zaremba on 4/7/23.
//

#ifndef SignalGenerator_h
#define SignalGenerator_h

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import "CommonTypes.h"
#import "SignalGeneratorEngineWrapper.h"

@interface SignalGenerator : NSObject {
    id delegate;
}

//-(void) outputDeviceChanged:(NSString*) _output_device;
-(void) setOutputDevice:(NSString*)output_device_uid active:(BOOL)isOn;
-(id) initWithSampleRate:(uint32_t)sample_rate numberOfChannels:(uint32_t)num_chans;
-(void) configureWithType:(SIG_GEN_TYPE) type frequency:(uint32_t)frequency;
-(void) adjustVolume:(float) volume;
-(void) startSignal;
-(void) stopSignal;
@end


#endif /* SignalGenerator_h */
