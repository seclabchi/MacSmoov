//
//  ProcessorSysInterface.m
//  MacSmoov
//
//  Created by Paul Zaremba on 4/16/23.
//

#import <cocoa/cocoa.h>
#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import <CommonCrypto/CommonDigest.h>
#import "ProcessorSysInterface.h"

#define BUFFER_SIZE 8192

@interface ProcessorSysInterface() {
    OSStatus theErr;
    AudioStreamBasicDescription asbd;
    AudioQueueRef inAQ;
    AudioQueueRef outAQ;
    AudioQueueBufferRef in_buffers[4];
    AudioQueueBufferRef out_buffers[4];
    AudioQueueBufferRef cur_out_buf;
    uint32_t cur_out_buf_index;
    uint32_t cur_in_buf_index;
    BOOL is_running;
    NSLock* lock_inout;
    AudioQueueBufferRef transfer_buffers[4];
    AudioQueueBufferRef head_of_free_output_buffers;
}

-(void) restart;
@end

@implementation ProcessorSysInterface

uint8_t inprocbufnum = 0;
uint8_t outprocbufnum = 0;

uint32_t inproccalls = 0;
uint32_t outproccalls = 0;

void audioprocOutQcb(void *inUserData, AudioQueueRef outAQ, AudioQueueBufferRef outBuffer) {
    ProcessorSysInterface* psi = (__bridge ProcessorSysInterface*) inUserData;
    //NSLog(@"audioprocOutQcb %p, # calls %d", outBuffer, outproccalls);
    outproccalls++;
    if(psi->is_running) {
        outBuffer->mUserData = psi->head_of_free_output_buffers;
        psi->head_of_free_output_buffers = outBuffer;
        
        AudioQueueEnqueueBuffer(outAQ, outBuffer, 0, nil);
    }
}

void audioprocInQcb(void *inUserData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer, const AudioTimeStamp* inStartTime, UInt32 npd, const AudioStreamPacketDescription* aspd) {
    
    ProcessorSysInterface* psi = (__bridge ProcessorSysInterface*) inUserData;
    //NSLog(@"audioprocInQcb %p, # calls %d", inBuffer, inproccalls);
    inproccalls++;
    
    
    if(psi->is_running) {
        //NSLog(@"cur_in/out_buf_index = %d/%d", psi->cur_in_buf_index, psi->cur_out_buf_index);
        if(psi->head_of_free_output_buffers != nil) {
            AudioQueueBufferRef outbuf = (AudioQueueBufferRef) psi->head_of_free_output_buffers;
            psi->head_of_free_output_buffers = outbuf->mUserData;
            outbuf->mAudioDataByteSize = inBuffer->mAudioDataByteSize;
            memcpy(outbuf->mAudioData, inBuffer->mAudioData, inBuffer->mAudioDataByteSize);
        }
        AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, nil);
    }
 }

-(id) init {
    self = [super init];
    if(self) {
        //input_device = @"AQDefaultInput";
        //input_device = @"AppleUSBAudioEngine:Plantronics:Plantronics Blackwire 3210 Series:FFE5F399D3F84D558CDC32EA0790A041:1";
        //input_device = @"AppleHDAEngineInput:1F,3,0,1,0:1";
        //output_device = @"AQDefaultOutput";
        //output_device = @"AppleHDAEngineOutput:1F,3,0,1,1:0";
        
        NSUserDefaults *prefs;
        NSUserDefaultsController *prefs_controller = [NSUserDefaultsController sharedUserDefaultsController];
        prefs = prefs_controller.defaults;
        
        output_device = [prefs stringForKey:@"OUTPUT_DEVICE"];
        
        if(nil == output_device) {
            output_device = @"AQDefaultOutput";
        }
        
        input_device = [prefs stringForKey:@"INPUT_DEVICE"];
        
        if(nil == input_device) {
            input_device = @"AQDefaultInput";
        }
        
        is_running = false;
        lock_inout = [[NSLock alloc] init];
        
        core = [[ProcessorCoreWrapper alloc] initWithSampleRate:48000 numberOfChannels:2 bufSize:BUFFER_SIZE];

        memset(&asbd, 0, sizeof(asbd));
        
        asbd.mFormatID = kAudioFormatLinearPCM;
        asbd.mBitsPerChannel = 32;
        asbd.mSampleRate = 48000;
        asbd.mFramesPerPacket = 1;
        asbd.mChannelsPerFrame = 2;
        asbd.mBytesPerPacket = 8;
        asbd.mBytesPerFrame = 8;
        asbd.mFormatFlags = kLinearPCMFormatFlagIsFloat;
        
        UInt32 audio_format_size = sizeof(asbd);
        theErr = AudioFormatGetProperty(kAudioFormatProperty_FormatInfo, 0, NULL, &audio_format_size, &asbd);
        assert(theErr == noErr);
        
        theErr = AudioQueueNewInput(&asbd, audioprocInQcb, (__bridge void*)self, NULL, NULL, 0, &inAQ);
        assert(theErr == noErr);
        
        theErr = AudioQueueGetProperty(inAQ, kAudioQueueProperty_StreamDescription, &asbd, &audio_format_size);
        assert(theErr == noErr);
        
        theErr = AudioQueueAllocateBuffer(inAQ, BUFFER_SIZE*sizeof(float), &in_buffers[0]);
        assert(theErr == noErr);
        in_buffers[0]->mAudioDataByteSize = BUFFER_SIZE*sizeof(float);
        
        theErr = AudioQueueAllocateBuffer(inAQ, BUFFER_SIZE*sizeof(float), &in_buffers[1]);
        assert(theErr == noErr);
        in_buffers[1]->mAudioDataByteSize = BUFFER_SIZE*sizeof(float);
        
        theErr = AudioQueueAllocateBuffer(inAQ, BUFFER_SIZE*sizeof(float), &in_buffers[2]);
        assert(theErr == noErr);
        in_buffers[2]->mAudioDataByteSize = BUFFER_SIZE*sizeof(float);
        
        theErr = AudioQueueAllocateBuffer(inAQ, BUFFER_SIZE*sizeof(float), &in_buffers[3]);
        assert(theErr == noErr);
        in_buffers[3]->mAudioDataByteSize = BUFFER_SIZE*sizeof(float);
        
        theErr = AudioQueueNewOutput(&asbd, audioprocOutQcb, (__bridge void*)self, NULL, NULL, 0, &outAQ);
        assert(theErr == noErr);
        
        theErr = AudioQueueAllocateBuffer(outAQ, BUFFER_SIZE*sizeof(float), &out_buffers[0]);
        assert(theErr == noErr);
        out_buffers[0]->mAudioDataByteSize = BUFFER_SIZE*sizeof(float);
        
        theErr = AudioQueueAllocateBuffer(outAQ, BUFFER_SIZE*sizeof(float), &out_buffers[1]);
        assert(theErr == noErr);
        out_buffers[1]->mAudioDataByteSize = BUFFER_SIZE*sizeof(float);
        
        theErr = AudioQueueAllocateBuffer(outAQ, BUFFER_SIZE*sizeof(float), &out_buffers[2]);
        assert(theErr == noErr);
        out_buffers[2]->mAudioDataByteSize = BUFFER_SIZE*sizeof(float);
        
        theErr = AudioQueueAllocateBuffer(outAQ, BUFFER_SIZE*sizeof(float), &out_buffers[3]);
        assert(theErr == noErr);
        out_buffers[3]->mAudioDataByteSize = BUFFER_SIZE*sizeof(float);
        
        
        
        cur_out_buf_index = 0;
        cur_in_buf_index = 0;

    }
    
    return self;
}

-(void) start {
        
    CFStringRef outdev_cfstr = (__bridge CFStringRef)output_device;
    theErr = AudioQueueSetProperty(outAQ, kAudioQueueProperty_CurrentDevice, &outdev_cfstr, sizeof(&outdev_cfstr));
    assert(theErr == noErr);

    CFStringRef indev_cfstr = (__bridge CFStringRef)input_device;
    theErr = AudioQueueSetProperty(inAQ, kAudioQueueProperty_CurrentDevice, &indev_cfstr, sizeof(&indev_cfstr));
    assert(theErr == noErr);
    
    
    theErr = AudioQueueEnqueueBuffer(outAQ, out_buffers[0], 0, NULL);
    assert(theErr == noErr);
    theErr = AudioQueueEnqueueBuffer(outAQ, out_buffers[1], 0, NULL);
    assert(theErr == noErr);
    theErr = AudioQueueEnqueueBuffer(outAQ, out_buffers[2], 0, NULL);
    assert(theErr == noErr);
    theErr = AudioQueueEnqueueBuffer(outAQ, out_buffers[3], 0, NULL);
    assert(theErr == noErr);
    
    theErr = AudioQueueEnqueueBuffer(inAQ, in_buffers[0], 0, NULL);
    assert(theErr == noErr);
    theErr = AudioQueueEnqueueBuffer(inAQ, in_buffers[1], 0, NULL);
    assert(theErr == noErr);
    theErr = AudioQueueEnqueueBuffer(inAQ, in_buffers[2], 0, NULL);
    assert(theErr == noErr);
    theErr = AudioQueueEnqueueBuffer(inAQ, in_buffers[3], 0, NULL);
    assert(theErr == noErr);
    
    //theErr = AudioQueuePrime(inAQ, 0, NULL);
    //assert(theErr == noErr);
    
    //theErr = AudioQueuePrime(outAQ, 0, NULL);
    //assert(theErr == noErr);
    
    is_running = YES;
    
    theErr = AudioQueueStart(inAQ, NULL);
    assert(theErr == noErr);
    
    theErr = AudioQueueStart(outAQ, NULL);
    assert(theErr == noErr);
}

-(void) stop {
    //AudioQueueFlush(outAQ);
    is_running = NO;
    
    
    
    theErr = AudioQueueStop(outAQ, TRUE);
    assert(theErr == noErr);
    
    //AudioQueueFlush(inAQ);
    theErr = AudioQueueStop(inAQ, TRUE);
    assert(theErr == noErr);
    
}

-(void) restart {
    NSLog(@"ProcessorSysInterface is restarting...");
}

-(void) inputDeviceChanged:(NSString*) _input_device {
    input_device = [NSString stringWithString:_input_device];
    
    if(is_running) {
        [self stop];
        [self start];
    }
}

-(void) outputDeviceChanged:(NSString*) _output_device {
    output_device = [NSString stringWithString:_output_device];
    
    if(is_running) {
        [self stop];
        [self start];
    }
    
    
 
}

@end
