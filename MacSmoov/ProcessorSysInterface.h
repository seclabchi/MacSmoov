//
//  ProcessorSysInterface.h
//  MacSmoov
//
//  Created by Paul Zaremba on 4/16/23.
//

#ifndef ProcessorSysInterface_h
#define ProcessorSysInterface_h

#import "ProcessorCoreWrapper.h"

@interface ProcessorSysInterface : NSObject {
    NSString* input_device;
    NSString* output_device;
    ProcessorCoreWrapper* core;
}

-(void) inputDeviceChanged:(NSString*) _input_device;
-(void) outputDeviceChanged:(NSString*) _output_device;
-(void) start;
-(void) stop;

@end

#endif /* ProcessorSysInterface_h */
