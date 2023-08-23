//
//  OSXAudioInterface.h
//  MacSmoov
//
//  Created by Paul Zaremba on 8/7/23.
//

#ifndef OSXAudioInterface_h
#define OSXAudioInterface_h

#import "AudioDevice.h"
#import "ProcessorCoreWrapper.h"

@interface OSXAudioInterface : NSObject {
    NSMutableDictionary* audio_input_device_dict;
    NSMutableDictionary* audio_output_device_dict;
    AudioComponentInstance audioUnitInput, audioUnitOutput;
    
}

@property (nonatomic, readonly, getter = get_current_input_device) AudioDevice* current_input_device;
@property (nonatomic, readonly, getter = get_current_output_device) AudioDevice* current_output_device;

@property (readonly, nonatomic, strong) NSMutableDictionary* input_devices;
@property (readonly, nonatomic, strong) NSMutableDictionary* output_devices;
@property (readonly, nonatomic, getter = get_sample_rate) NSUInteger sample_rate;
@property (readonly, nonatomic, getter = get_num_channels) NSUInteger num_channels;
@property (readonly, nonatomic, getter = get_buffer_size) NSUInteger buffer_size;

- (id) initWithCurrentInputDevice:(AudioDevice*)in_dev OutputDevice:(AudioDevice*)out_dev;
- (void) set_processor_hook:(PROCESSOR_CORE_HOOK)hook;
- (OSStatus) discoverDevices;
- (OSStatus) set_input_device:(AudioDevice*)input_dev;
- (OSStatus) set_output_device:(AudioDevice*)output_dev;
- (OSStatus) start;
- (OSStatus) stop;

- (void) writeAudioDataToFile:(AudioBufferList*)ioData;

@end



#endif /* OSXAudioInterface_h */
