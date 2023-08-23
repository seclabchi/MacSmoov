//
//  ProcessorSysInterface.h
//  MacSmoov
//
//  Created by Paul Zaremba on 4/16/23.
//

#ifndef ProcessorSysInterface_h
#define ProcessorSysInterface_h

#import <cocoa/cocoa.h>
#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import <CommonCrypto/CommonDigest.h>
#import "ProcessorCoreWrapper.h"

@interface ProcessorSysInterface : NSObject {
    ProcessorCoreWrapper* core;
}

-(id) initWithSampleRate:(NSUInteger)sample_rate numberOfChannels:(UInt8)channels bufferSize:(NSUInteger)bytesPerChannel;
-(void) processWithInput:(AudioBufferList*)buf_list;
-(void) getMainInLevelsLrms:(float*)lrms Rrms:(float*)rrms Lpeak:(float*)lpeak Rpeak:(float*)rpeak;
-(void) setMainInGainDBL:(float)mainInL R:(float)mainInR;

@end

#endif /* ProcessorSysInterface_h */
