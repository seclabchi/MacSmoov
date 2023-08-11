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

#define OUTPUT_BUS 0
#define INPUT_BUS 1
#define SAMPLE_RATE 44100
#define NUM_CHANNELS 2  // 1 is mono: 2 is stereo
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

AudioComponent audioUnitInput;
NSFileHandle* capture_file_handle;

void checkStatus(int status) {
    if (status) {
        NSError *error = [NSError errorWithDomain:NSCocoaErrorDomain code:status userInfo:nil];
        NSLog(@"Error %d encountered: %@", status, error.localizedDescription);
        assert(status == noErr);
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

static OSStatus recordingCallback(void *inRefCon,
                                  AudioUnitRenderActionFlags *ioActionFlags,
                                  const AudioTimeStamp *inTimeStamp,
                                  UInt32 inBusNumber,
                                  UInt32 inNumberFrames,
                                  AudioBufferList *ioData) {
 
    OSStatus err = 0;
    AudioStreamBasicDescription stream_format_prop;
    UInt32 stream_format_prop_size = sizeof(stream_format_prop);
    Boolean propWriteable = NO;
    

    
    // TODO: Use inRefCon to access our interface object to do stuff
    // Then, use inNumberFrames to figure out how much data is available, and make
    // that much space available in buffers in an AudioBufferList.
    
    
    
    err = 0;
 
    OSXAudioInterface* osxai = (__bridge OSXAudioInterface*) inRefCon;
    
    err = AudioUnitGetPropertyInfo(osxai->audioUnitInput, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, INPUT_BUS,
                                   &stream_format_prop_size, &propWriteable);
    
    checkStatus(err);
    
    err = AudioUnitGetProperty(osxai->audioUnitInput, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, INPUT_BUS,
                               &stream_format_prop, &stream_format_prop_size);
    
    checkStatus(err);
    
    
    // Then:
    // Obtain recorded samples
 
    UInt32 buf_size_in_bytes = inNumberFrames * sizeof(Float32);
    size_t buf_list_size = offsetof(AudioBufferList, mBuffers[0]) + (sizeof(AudioBuffer) * 2);
    AudioBufferList* buf_list = (AudioBufferList*)calloc(1, buf_list_size);
    buf_list->mNumberBuffers = 2;
    
    buf_list->mBuffers[0].mNumberChannels = 1;
    buf_list->mBuffers[0].mDataByteSize = buf_size_in_bytes;
    buf_list->mBuffers[0].mData = calloc(1, buf_size_in_bytes);
    
    buf_list->mBuffers[1].mNumberChannels = 1;
    buf_list->mBuffers[1].mDataByteSize = buf_size_in_bytes;
    buf_list->mBuffers[1].mData = calloc(1, buf_size_in_bytes);
    
    NSLog(@"recordingCallback - inTimeStamp %llu, inNumberFrames %d, buf_list %p", inTimeStamp->mHostTime, inNumberFrames, buf_list);
    
    err = AudioUnitRender(osxai->audioUnitInput,
                             ioActionFlags,
                             inTimeStamp,
                             inBusNumber,
                             inNumberFrames,
                             buf_list);
    checkStatus(err);
 
    // Now, we have the samples we just read sitting in buffers in bufferList
    
    Float32* outdata = (Float32*)calloc(1, 2 * inNumberFrames * sizeof(Float32));
    Float32* buf00 = (Float32*)(buf_list->mBuffers[0].mData);
    Float32* buf01 = (Float32*)(buf_list->mBuffers[1].mData);
    UInt32 j = 0;
    
    for(UInt32 i = 0; i < inNumberFrames; i++) {
        outdata[j] = buf00[i];
        outdata[j+1] = buf01[i];
        j+=2;
    }
    
    NSData* data = [NSData dataWithBytes:outdata length:2*inNumberFrames*sizeof(Float32)];
    [capture_file_handle seekToEndOfFile];
    [capture_file_handle writeData:data];
    
    free(outdata);
    
    free(buf_list->mBuffers[0].mData);
    free(buf_list->mBuffers[1].mData);
    free(buf_list);
    
    return noErr;
}

/**
 This callback is called when the audioUnit needs new data to play through the
 speakers. If you don't have any, just don't write anything in the buffers
 */
static OSStatus playbackCallback(void *inRefCon,
                                 AudioUnitRenderActionFlags *ioActionFlags,
                                 const AudioTimeStamp *inTimeStamp,
                                 UInt32 inBusNumber,
                                 UInt32 inNumberFrames,
                                 AudioBufferList *ioData) {
    // Notes: ioData contains buffers (may be more than one!)
    // Fill them up as much as you can. Remember to set the size value in each buffer to match how
    // much data is in the buffer.
    
    NSLog(@"playbackCallback - inTimeStamp %llu, inNumberFrames %d, ioData %p", inTimeStamp->mHostTime, inNumberFrames, ioData);
    
    OSXAudioInterface *iosAudio = (__bridge OSXAudioInterface*)inRefCon;
    //NSData *data = [iosAudio audioDataPCMLen:ioData->mBuffers[0].mDataByteSize];
    
    //data = iosAudio.tempData;
    //NSUInteger len = data.length;
    
    AudioBuffer buffer;
    
    for (int i=0; i < ioData->mNumberBuffers; i++) { // in practice we will only ever have 1 buffer, since audio format is mono
        buffer = ioData->mBuffers[i];
        //        NSLog(@"  Buffer %d has %d channels and wants %d bytes of data.", i, buffer.mNumberChannels, buffer.mDataByteSize);
        
        // copy temporary buffer data to output buffer
        //NSUInteger size = min(buffer.mDataByteSize, len); // dont copy more data then we have, or then fits
        //memcpy(buffer.mData, data.bytes, size);
        //buffer.mDataByteSize = (UInt32)size; // indicate how much data we wrote in the buffer
        
        // uncomment to hear random noise
//         UInt16 *frameBuffer = buffer.mData;
//         for (int j = 0; j < inNumberFrames; j++) {
//         frameBuffer[j] = rand();
//         }
    }
    
    
    
    return noErr;
}

- (OSStatus) go {
    OSStatus err = 0;
    AudioObjectPropertyAddress addr;
    AudioDeviceID deviceID = 0;
    UInt32 size = sizeof(AudioDeviceID);
    AudioComponent comp;
    UInt32 enableIO;
    
    NSString* cap_file = @"macsmoov_capture.pcm";
    NSFileManager *fileMan = [NSFileManager defaultManager];
    if (![fileMan fileExistsAtPath:cap_file])
    {
        [fileMan createFileAtPath:cap_file contents:nil attributes:nil];
    }
    
    capture_file_handle = [NSFileHandle fileHandleForUpdatingAtPath:cap_file];
    
    //STEP 1:  Obtain the system remote I/O hardware device
    
    AudioComponentDescription desc = {
        .componentType = kAudioUnitType_Output,
        .componentSubType = kAudioUnitSubType_HALOutput,
        .componentManufacturer = kAudioUnitManufacturer_Apple,
        .componentFlags = 0,
        .componentFlagsMask = 0
    };
    
    comp = AudioComponentFindNext(NULL, &desc);
    if (comp == NULL)
    {
        return -1;
    }
    
    err = AudioComponentInstanceNew(comp, &audioUnitInput);
    checkStatus(err);
    
    //STEP 2:  Enable desired I/O
    
    //When using AudioUnitSetProperty the 4th parameter in the method
    //refer to an AudioUnitElement. When using an AudioOutputUnit
    //the input element will be '1' and the output element will be '0'.

    enableIO = 1;
    err = AudioUnitSetProperty(audioUnitInput,
                 kAudioOutputUnitProperty_EnableIO,
                 kAudioUnitScope_Input,
                 INPUT_BUS, // input element
                 &enableIO,
                 sizeof(enableIO));
    
    checkStatus(err);
    
    enableIO = 0;
    err = AudioUnitSetProperty(audioUnitInput,
                 kAudioOutputUnitProperty_EnableIO,
                 kAudioUnitScope_Output,
                 OUTPUT_BUS,   //output element
                 &enableIO,
                 sizeof(enableIO));
    
    checkStatus(err);
    
    //STEP 3: Setup the input AudioUnit
    
    addr.mSelector = kAudioHardwarePropertyDefaultInputDevice;
    addr.mScope = kAudioObjectPropertyScopeGlobal;
    addr.mElement = kAudioObjectPropertyElementMaster;

    err = AudioObjectGetPropertyData(kAudioObjectSystemObject, &addr, 0, NULL, &size, &deviceID);
    checkStatus(err);
    
    if (err == noErr) {
        err = AudioUnitSetProperty(audioUnitInput, kAudioOutputUnitProperty_CurrentDevice, kAudioUnitScope_Global, OUTPUT_BUS, &deviceID, size);
    }
    
    checkStatus(err);
    
    AudioStreamBasicDescription stream_format_prop;
    UInt32 stream_format_prop_size = sizeof(stream_format_prop);
    Boolean propWriteable = NO;
    
    err = AudioUnitGetPropertyInfo(audioUnitInput, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, INPUT_BUS,
                                   &stream_format_prop_size, &propWriteable);
    
    checkStatus(err);
    
    err = AudioUnitGetProperty(audioUnitInput, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, INPUT_BUS,
                               &stream_format_prop, &stream_format_prop_size);
    
    checkStatus(err);
    
    int m_valueCount = deviceID / sizeof(AudioValueRange) ;
    NSLog(@"Available %d Sample Rates\n",m_valueCount);
    
    NSLog(@"DeviceName: %@",[self deviceName:deviceID]);
    NSLog(@"BufferSize: %d",[self bufferSize:deviceID]);
    
    //Get some information about the default input device
    
    Float64 propValFloat = 0.0;
    UInt32 propSize = sizeof(propValFloat);
    
    
    addr.mSelector = kAudioUnitProperty_SampleRate;
    addr.mScope = kAudioObjectPropertyScopeInput;
    addr.mElement = kAudioObjectPropertyElementMaster;
    

    //STEP 4: Setup the desired input audio format
    AudioStreamBasicDescription audioFormat;
    audioFormat.mSampleRate             = SAMPLE_RATE;
    audioFormat.mFormatID               = kAudioFormatLinearPCM;
    audioFormat.mFormatFlags            = kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked | kAudioFormatFlagIsNonInterleaved;
    audioFormat.mFramesPerPacket        = 1;
    audioFormat.mChannelsPerFrame       = NUM_CHANNELS;
    audioFormat.mBitsPerChannel         = 8 * sizeof(Float32);
    audioFormat.mBytesPerPacket         = sizeof(Float32);
    audioFormat.mBytesPerFrame          = sizeof(Float32);
    
    size = sizeof(AudioStreamBasicDescription);
    
    // Apply format
    err = AudioUnitSetProperty(audioUnitInput,
                              kAudioUnitProperty_StreamFormat,
                              kAudioUnitScope_Input,
                              OUTPUT_BUS,
                              &audioFormat,
                              size);
    checkStatus(err);
    
    err = AudioUnitSetProperty(audioUnitInput,
                              kAudioUnitProperty_StreamFormat,
                              kAudioUnitScope_Output,
                              INPUT_BUS,
                              &audioFormat,
                              size);
    checkStatus(err);
    
    err = AudioUnitGetPropertyInfo(audioUnitInput, kAudioUnitProperty_SampleRate, kAudioUnitScope_Input, OUTPUT_BUS,
                                   &propSize, &propWriteable);
    
    checkStatus(err);
    
    err = AudioUnitGetProperty(audioUnitInput, kAudioUnitProperty_SampleRate, kAudioUnitScope_Input, OUTPUT_BUS,
                               &propValFloat, &propSize);
    
    checkStatus(err);
    
    err = AudioUnitGetPropertyInfo(audioUnitInput, kAudioUnitProperty_SampleRate, kAudioUnitScope_Output, INPUT_BUS,
                                   &propSize, &propWriteable);
    
    checkStatus(err);
    
    err = AudioUnitGetProperty(audioUnitInput, kAudioUnitProperty_SampleRate, kAudioUnitScope_Output, INPUT_BUS,
                               &propValFloat, &propSize);
    
    checkStatus(err);
    
    //STEP 5: Setup input callback
    AURenderCallbackStruct callbackStruct;
    callbackStruct.inputProc = recordingCallback;
    callbackStruct.inputProcRefCon = (__bridge void * _Nullable)(self);
    
    size = sizeof(AURenderCallbackStruct);
    err = AudioUnitSetProperty(audioUnitInput,
                              kAudioOutputUnitProperty_SetInputCallback,
                              kAudioUnitScope_Global,
                              OUTPUT_BUS,
                              &callbackStruct,
                              size);
    checkStatus(err);
    
    //STEP 6: Setup render callback  - THIS IS FOR PLAYING SHIT ONLY!!!!!!!!
    //callbackStruct.inputProc = playbackCallback;
    //callbackStruct.inputProcRefCon = (__bridge void * _Nullable)(self);
    
    //size = sizeof(AURenderCallbackStruct);
    //err = AudioUnitSetProperty(audioUnitInput,
    //                          kAudioUnitProperty_SetRenderCallback,
    //                          kAudioUnitScope_Global,
    //                          1,
    //                          &callbackStruct,
    //                          size);
    //checkStatus(err);
    
    //STEP 7: Initialize and start the AudioUnit
    err = AudioUnitInitialize(audioUnitInput);
    checkStatus(err);
    
    err = AudioOutputUnitStart(audioUnitInput);
    checkStatus(err);
    
    return err;
}

- (OSStatus) set_input_device:(AudioDevice*)input_dev {
    UInt32 size = sizeof(AudioDeviceID);
    AudioDeviceID deviceID = input_dev.device_id;
    OSStatus err;
    
    NSLog(@"Changing input device to %@ (%@)", input_dev.device_name, input_dev.device_uid);
    
    size = sizeof(deviceID);
    err = AudioUnitSetProperty(audioUnitInput, kAudioOutputUnitProperty_CurrentDevice, kAudioUnitScope_Global, 0, &deviceID, size);

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
    err = AudioUnitGetProperty(audioUnitInput, kAudioOutputUnitProperty_CurrentDevice, kAudioUnitScope_Global, 0, &deviceID, &size);

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
    err = AudioUnitSetProperty(audioUnitOutput, kAudioOutputUnitProperty_CurrentDevice, kAudioUnitScope_Global, 0, &deviceID, size);
    
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

-(NSString *)deviceName:(AudioDeviceID)devID
{
    // Check name
    AudioObjectPropertyAddress address;
    
    address.mSelector = kAudioObjectPropertyName;
    address.mScope = kAudioObjectPropertyScopeGlobal;
    address.mElement = kAudioObjectPropertyElementMaster;

    CFStringRef name;
    UInt32 stringsize = sizeof(CFStringRef);
    
    AudioObjectGetPropertyData(devID, &address, 0, nil, &stringsize, &name);
    
    return (__bridge NSString *)(name);
}

-(UInt32)bufferSize:(AudioDeviceID)devID
{
    // Check buffer size
    AudioObjectPropertyAddress address;
    
    address.mSelector = kAudioDevicePropertyBufferFrameSize;
    address.mScope = kAudioObjectPropertyScopeGlobal;
    address.mElement = kAudioObjectPropertyElementMaster;
    
    UInt32 buf = 0;
    UInt32 bufSize = sizeof(UInt32);
    
    AudioObjectGetPropertyData(devID, &address, 0, nil, &bufSize, &buf);
    
    return buf;
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
