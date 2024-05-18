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
    
}

@property (nonatomic, readonly, getter = get_current_input_device) AudioDevice* current_input_device;
@property (nonatomic, readonly, getter = get_current_output_device) AudioDevice* current_output_device;

@property (readonly, nonatomic, strong) NSMutableDictionary* input_devices;
@property (readonly, nonatomic, strong) NSMutableDictionary* output_devices;
@property (readonly, nonatomic, getter = get_sample_rate) uint32_t sample_rate;
@property (readonly, nonatomic, getter = get_num_channels) uint32_t num_channels;
@property (readonly, nonatomic, getter = get_buffer_size) uint32_t buffer_size;

- (id) init;
- (void) set_processor_hook:(PROCESSOR_CORE_HOOK)hook;
- (OSStatus) discoverDevices;
- (void) get_all_input_device_names:(NSMutableArray*)in_devs;
- (void) get_all_output_device_names:(NSMutableArray*)out_devs;
- (OSStatus) set_input_device_from_name:(NSString*)input_dev;
- (OSStatus) set_output_device_from_name:(NSString*)output_dev;
- (OSStatus) start;
- (OSStatus) stop;

//- (void) writeAudioDataToFile:(AudioBufferList*)ioData;

@end



#endif /* OSXAudioInterface_h */
