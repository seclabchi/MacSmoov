//
//  OSXAudioInterface.m
//  MacSmoov
//
//  Created by Paul Zaremba on 8/7/23.
//

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import "OSXAudioInterface.h"
#import "AudioDevice.h"

#define kOutputBus 0
#define kInputBus 1
#define SampleRate 48000
#define numberOfChannel 2  // 1 is mono: 2 is stereo
#define MAX_AUDIO_DEVICES 256

bool deviceHasBuffersInScope(AudioObjectID deviceID, AudioObjectPropertyScope scope);

@interface OSXAudioInterface ()

@property (nonatomic, strong) NSMutableDictionary* input_devices;
@property (nonatomic, strong) NSMutableDictionary* output_devices;
@property (nonatomic, strong) AudioDevice* current_input_device;
@property (nonatomic, strong) AudioDevice* current_output_device;

@end

@implementation OSXAudioInterface

@synthesize input_devices;
@synthesize output_devices;

void checkStatus(int status) {
    if (status) {
        NSError *error = [NSError errorWithDomain:NSOSStatusErrorDomain code:status userInfo:nil];
        NSLog(@"Error %d encountered: %@", status, error.localizedDescription);
    }
}

-(id) init {
    self = [super init];
    
    if(self) {
        audio_input_device_dict = [[NSMutableDictionary alloc] init];
        audio_output_device_dict = [[NSMutableDictionary alloc] init];
        self.input_devices = audio_input_device_dict;
        self.output_devices = audio_output_device_dict;
    }
    
    return self;
}

-(OSStatus) initialize_audio_units {
    AudioComponentDescription desc;
    AudioComponent comp;
    
    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_HALOutput;//kAudioUnitSubType_VoiceProcessingIO;
    
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;
    
    comp = AudioComponentFindNext(NULL, &desc);
    if (comp == NULL)
    {
        return -1;
    }
    
    OSStatus err = AudioComponentInstanceNew(comp, &audioUnitInput);
    checkStatus(err);
    
    err = AudioComponentInstanceNew(comp, &audioUnitOutput);
    checkStatus(err);
    
    UInt32 enableIO;
        
    //When using AudioUnitSetProperty the 4th parameter in the method
    //refer to an AudioUnitElement. When using an AudioOutputUnit
    //the input element will be '1' and the output element will be '0'.
    
    enableIO = 1;
    err = AudioUnitSetProperty(audioUnitInput,
                         kAudioOutputUnitProperty_EnableIO,
                         kAudioUnitScope_Input,
                         kInputBus, // input element
                         &enableIO,
                         sizeof(enableIO));
    
    checkStatus(err);
    
    err = AudioUnitSetProperty(audioUnitOutput,
                         kAudioOutputUnitProperty_EnableIO,
                         kAudioUnitScope_Output,
                         kOutputBus, // input element
                         &enableIO,
                         sizeof(enableIO));
    
    checkStatus(err);
    
    enableIO = 0;
    err = AudioUnitSetProperty(audioUnitInput,
                         kAudioOutputUnitProperty_EnableIO,
                         kAudioUnitScope_Output,
                         kOutputBus,   //output element
                         &enableIO,
                         sizeof(enableIO));
    checkStatus(err);
    
    err = AudioUnitSetProperty(audioUnitOutput,
                         kAudioOutputUnitProperty_EnableIO,
                         kAudioUnitScope_Input,
                         kInputBus,   //output element
                         &enableIO,
                         sizeof(enableIO));
    checkStatus(err);
    
    return err;
}



- (OSStatus) set_input_device:(AudioDevice*)input_dev {
    UInt32 size = sizeof(AudioDeviceID);
    AudioDeviceID deviceID = input_dev.device_id;
    OSStatus err;
    
    NSLog(@"Changing input device to %@ (%@)", input_dev.device_name, input_dev.device_uid);
    
    size = sizeof(deviceID);
    err = AudioUnitSetProperty(audioUnitInput, kAudioOutputUnitProperty_CurrentDevice, kAudioObjectPropertyScopeInput, 0, &deviceID, size);

    checkStatus(err);
    
    if(!err) {
        self.current_input_device = input_dev;
    }
    
    return err;
}

- (AudioDevice*) get_current_input_device {
    UInt32 size = sizeof(AudioDeviceID);
    AudioDeviceID deviceID = 0;
    OSStatus err;
    AudioDevice* ad = nil;
    
    size = sizeof(deviceID);
    err = AudioUnitGetProperty(audioUnitInput, kAudioOutputUnitProperty_CurrentDevice, kAudioObjectPropertyScopeInput, 0, &deviceID, &size);

    checkStatus(err);
    
    if(!err) {
        ad = [input_devices objectForKey:[NSNumber numberWithUnsignedInt:deviceID]];
        _current_input_device = ad;
    }
    
    return ad;
}

- (OSStatus) set_output_device:(AudioDevice*)output_dev {
    NSLog(@"Changing output device to %@ (%@)", output_dev.device_name, output_dev.device_uid);
    OSStatus err;
    UInt32 size;
    AudioDeviceID deviceID = output_dev.device_id;
    
    size = sizeof(deviceID);
    err = AudioUnitSetProperty(audioUnitOutput, kAudioOutputUnitProperty_CurrentDevice, kAudioObjectPropertyScopeOutput, 0, &deviceID, size);
    
    checkStatus(err);
    
    if(!err) {
        self.current_output_device = output_dev;
    }
    
    return err;
}

- (AudioDevice*) get_current_output_device {
    UInt32 size = sizeof(AudioDeviceID);
    AudioDeviceID deviceID = 0;
    OSStatus err;
    AudioDevice* ad = nil;
    
    size = sizeof(deviceID);
    err = AudioUnitGetProperty(audioUnitOutput, kAudioOutputUnitProperty_CurrentDevice, kAudioObjectPropertyScopeOutput, 0, &deviceID, &size);

    checkStatus(err);
    
    if(!err) {
        ad = [output_devices objectForKey:[NSNumber numberWithUnsignedInt:deviceID]];
        _current_output_device = ad;
    }
    
    return ad;
}

 - (OSStatus) discoverDevices {
    OSStatus err = 0;
    
    NSLog(@"Discovering audio devices...");

    AudioDeviceID devices[MAX_AUDIO_DEVICES];
    UInt32 arraySize = sizeof(devices);

    AudioObjectPropertyAddress thePropertyAddress = { kAudioHardwarePropertyDevices,
                                                    kAudioObjectPropertyScopeGlobal,
                                                    kAudioObjectPropertyElementMain };

    AudioObjectGetPropertyData(kAudioObjectSystemObject,
                            &thePropertyAddress,
                            0,
                            NULL,
                            &arraySize,
                            &devices);

    for(uint32_t i = 0; i < arraySize/sizeof(AudioDeviceID); i++) {
        OSStatus theErr;
        UInt32 prop_size = 0;
         
        bool isOutput = deviceHasBuffersInScope(devices[i], kAudioObjectPropertyScopeOutput);
        bool isInput = deviceHasBuffersInScope(devices[i], kAudioObjectPropertyScopeInput);

        if(isOutput || isInput) {
     
            AudioObjectPropertyAddress prop_addr = { kAudioDevicePropertyDeviceUID,
                                                    kAudioObjectPropertyScopeGlobal,
                                                    kAudioObjectPropertyElementMain };

            theErr = AudioObjectGetPropertyDataSize(devices[i],
                                                    &prop_addr,
                                                    0,
                                                    NULL,
                                                    &prop_size);

            assert(theErr == noErr);
     
            CFStringRef device_uid_cfstr = nil;
            prop_size = sizeof(device_uid_cfstr);

            theErr = AudioObjectGetPropertyData(devices[i],
                                                &prop_addr,
                                                0,
                                                NULL,
                                                &prop_size,
                                                (void*)&device_uid_cfstr);
     
            //NSLog(@"OUTPUT DEVICE UID: %@", (__bridge NSString*) device_uid_cfstr);
     
            prop_addr.mSelector = kAudioObjectPropertyName;
     
            CFStringRef device_name_cfstr = nil;
            theErr = AudioObjectGetPropertyData(devices[i],
                                                &prop_addr,
                                                0,
                                                NULL,
                                                &prop_size,
                                                (void*)&device_name_cfstr);
     
            AudioDevice* ad = [[AudioDevice alloc] init];
            ad.device_id = devices[i];
            ad.device_uid = device_uid_cfstr;
            ad.device_name = device_name_cfstr;
            
            if(isInput) {
                NSNumber* device_id_number = [NSNumber numberWithUnsignedInt:devices[i]];
                [audio_input_device_dict setObject:ad forKey:device_id_number];
            }
            
            if(isOutput) {
                NSNumber* device_id_number = [NSNumber numberWithUnsignedInt:devices[i]];
                [audio_output_device_dict setObject:ad forKey:device_id_number];
            }
        }
    }

    NSLog(@"Finished discovering audio devices.");
     
     for(NSNumber* key in audio_input_device_dict) {
         AudioDevice* dev = audio_input_device_dict[key];
         NSLog(@"Audio Input Device Discovered: ID: %d, Name: %@, UID: %@", dev.device_id, dev.device_name, dev.device_uid);
     }
     
     for(NSNumber* key in audio_output_device_dict) {
         AudioDevice* dev = audio_output_device_dict[key];
         NSLog(@"Audio Output Device Discovered: ID: %d, Name: %@, UID: %@", dev.device_id, dev.device_name, dev.device_uid);
     }
     
    return err;
}

bool deviceHasBuffersInScope(AudioObjectID deviceID, AudioObjectPropertyScope scope) {
    if(deviceID == 0) {
        return NO;
    }

    AudioObjectPropertyAddress propertyAddress = {
        .mSelector  = kAudioDevicePropertyStreamConfiguration,
        .mScope     = scope,
        .mElement   = kAudioObjectPropertyElementWildcard
    };

    UInt32 dataSize = 0;
    OSStatus result = AudioObjectGetPropertyDataSize(deviceID, &propertyAddress, 0, NULL, &dataSize);
    if(result != kAudioHardwareNoError) {
        return NO;
    }

    AudioBufferList *bufferList = malloc(dataSize);
    if(!bufferList) {
        return NO;
    }

    result = AudioObjectGetPropertyData(deviceID, &propertyAddress, 0, NULL, &dataSize, bufferList);
    if(result != kAudioHardwareNoError) {
        free(bufferList);
        return NO;
    }

    BOOL supportsScope = bufferList->mNumberBuffers > 0;
    free(bufferList);

    return supportsScope;
}

/*- (OSStatus) deprecated {
    
    AudioObjectPropertyAddress addr;
    UInt32 size = sizeof(AudioDeviceID);
    AudioDeviceID deviceID = 0;
    
    AudioComponentDescription desc;
    AudioComponent comp;
    
    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_HALOutput;//kAudioUnitSubType_VoiceProcessingIO;
    
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;
    
    comp = AudioComponentFindNext(NULL, &desc);
    if (comp == NULL)
    {
        return -1;
    }
    
    OSStatus err = AudioComponentInstanceNew(comp, &audioUnit);
    checkStatus(err);
    
    UInt32 enableIO;
        
    //When using AudioUnitSetProperty the 4th parameter in the method
    //refer to an AudioUnitElement. When using an AudioOutputUnit
    //the input element will be '1' and the output element will be '0'.
    
 
    enableIO = 1;
    err = AudioUnitSetProperty(audioUnit,
                         kAudioOutputUnitProperty_EnableIO,
                         kAudioUnitScope_Input,
                         kInputBus, // input element
                         &enableIO,
                         sizeof(enableIO));
    
    checkStatus(err);
    
    enableIO = 0;
    err = AudioUnitSetProperty(audioUnit,
                         kAudioOutputUnitProperty_EnableIO,
                         kAudioUnitScope_Output,
                         kOutputBus,   //output element
                         &enableIO,
                         sizeof(enableIO));
    checkStatus(err);
    
    //addr.mSelector = kAudioHardwarePropertyDefaultInputDevice;
    //addr.mScope = kAudioObjectPropertyScopeGlobal;
    //addr.mElement = kAudioObjectPropertyElementMaster;

    //err = AudioObjectGetPropertyData(kAudioObjectSystemObject, &addr, 0, NULL, &size, &deviceID);
    
    AudioObjectID device = 0;
    
    addr.mSelector = kAudioHardwarePropertyTranslateUIDToDevice;
    addr.mScope = kAudioObjectPropertyScopeGlobal;
    addr.mElement = kAudioObjectPropertyElementMaster;

    err = AudioObjectGetPropertyData(kAudioObjectSystemObject, &addr, 0, NULL, &size, &device);
    
    checkStatus(err);

    if (err == noErr) {
        err = AudioUnitSetProperty(audioUnit, kAudioOutputUnitProperty_CurrentDevice, kAudioUnitScope_Global, 0, &deviceID, size);
    }

    checkStatus(err);

    return err;
}
*/
@end
