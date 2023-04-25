//
//  ProcessorSysInterface.m
//  MacSmoov
//
//  Created by Paul Zaremba on 4/16/23.
//

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import "ProcessorSysInterface.h"

#define BUFFER_SIZE 2048

@interface ProcessorSysInterface() {
    OSStatus theErr;
    AudioStreamBasicDescription asbd;
    AudioQueueRef inAQ;
    AudioQueueRef outAQ;
    AudioQueueBufferRef in_buffers[2];
    AudioQueueBufferRef out_buffers[2];
    float* procbuf0;
    float* procbuf1;
    BOOL is_running;
}

-(void) restart;
@end

@implementation ProcessorSysInterface

uint8_t inprocbufnum = 0;
uint8_t outprocbufnum = 0;

void audioprocInQcb(void *inUserData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer, const AudioTimeStamp* inStartTime, UInt32 npd, const AudioStreamPacketDescription* aspd) {
    ProcessorSysInterface* psi = (__bridge ProcessorSysInterface*) inUserData;
    //NSLog(@"Got capture buffer!");
    float* indata = (float*)(inBuffer->mAudioData);
    //NSLog(@"Capture start time: sample time %f, host time %llu, flags 0x%02X", inStartTime->mSampleTime, inStartTime->mHostTime, inStartTime->mFlags);
    
    if(inprocbufnum == 0) {
        memcpy(psi->procbuf0, indata, BUFFER_SIZE*sizeof(float));
        inprocbufnum = 1;
    }
    else {
        memcpy(psi->procbuf1, indata, BUFFER_SIZE*sizeof(float));
        inprocbufnum = 0;
    }
    AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, NULL);
}

void audioprocOutQcb(void *inUserData, AudioQueueRef outAQ, AudioQueueBufferRef inBuffer) {
    ProcessorSysInterface* psi = (__bridge ProcessorSysInterface*) inUserData;
    //NSLog(@"Got capture buffer!");
    
    float* outdata = (float*)(inBuffer->mAudioData);
    
    if(outprocbufnum == 0) {
        memcpy(outdata, psi->procbuf0, BUFFER_SIZE*sizeof(float));
        outprocbufnum = 1;
    }
    else {
        memcpy(outdata, psi->procbuf1, BUFFER_SIZE*sizeof(float));
        outprocbufnum = 0;
    }
    AudioQueueEnqueueBuffer(outAQ, inBuffer, 0, NULL);
    
}

-(id) init {
    self = [super init];
    if(self) {
        input_device = @"AQDefaultInput";
        //input_device = @"AppleUSBAudioEngine:Plantronics:Plantronics Blackwire 3210 Series:FFE5F399D3F84D558CDC32EA0790A041:1";
        //input_device = @"AppleHDAEngineInput:1F,3,0,1,0:1";
        output_device = @"AQDefaultOutput";
        //output_device = @"AppleHDAEngineOutput:1F,3,0,1,1:0";
        is_running = false;
        
        procbuf0 = (float*)malloc(BUFFER_SIZE * sizeof(float));
        procbuf1 = (float*)malloc(BUFFER_SIZE * sizeof(float));
        
        memset(procbuf0, 0, BUFFER_SIZE * sizeof(float));
        memset(procbuf1, 0, BUFFER_SIZE * sizeof(float));
        
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
        
        
        
        theErr = AudioQueueNewOutput(&asbd, audioprocOutQcb, (__bridge void*)self, NULL, NULL, 0, &outAQ);
        assert(theErr == noErr);
        
        theErr = AudioQueueAllocateBuffer(outAQ, BUFFER_SIZE*sizeof(float), &out_buffers[0]);
        assert(theErr == noErr);
        out_buffers[0]->mAudioDataByteSize = BUFFER_SIZE*sizeof(float);
        
        
        
        theErr = AudioQueueAllocateBuffer(outAQ, BUFFER_SIZE*sizeof(float), &out_buffers[1]);
        assert(theErr == noErr);
        out_buffers[1]->mAudioDataByteSize = BUFFER_SIZE*sizeof(float);
        
        
        
    }
    
    return self;
}

-(void) start {
    CFStringRef outdev_cfstr = (__bridge CFStringRef)output_device;
    theErr = AudioQueueSetProperty(outAQ, kAudioQueueProperty_CurrentDevice, &outdev_cfstr, sizeof(&outdev_cfstr));
    assert(theErr == noErr);
    
    theErr = AudioQueueEnqueueBuffer(inAQ, in_buffers[0], 0, NULL);
    assert(theErr == noErr);
    theErr = AudioQueueEnqueueBuffer(inAQ, in_buffers[1], 0, NULL);
    assert(theErr == noErr);
    theErr = AudioQueueEnqueueBuffer(outAQ, out_buffers[0], 0, NULL);
    assert(theErr == noErr);
    theErr = AudioQueueEnqueueBuffer(outAQ, out_buffers[1], 0, NULL);
    assert(theErr == noErr);
    
    audioprocOutQcb((__bridge void*) self, outAQ, out_buffers[0]);
    audioprocOutQcb((__bridge void*) self, outAQ, out_buffers[1]);
        
    theErr = AudioQueueStart(outAQ, NULL);
    assert(theErr == noErr);
    CFStringRef indev_cfstr = (__bridge CFStringRef)input_device;
    theErr = AudioQueueSetProperty(inAQ, kAudioQueueProperty_CurrentDevice, &indev_cfstr, sizeof(&indev_cfstr));
    assert(theErr == noErr);
    theErr = AudioQueueStart(inAQ, NULL);
    assert(theErr == noErr);
    is_running = YES;
}

-(void) stop {
    AudioQueueFlush(outAQ);
    theErr = AudioQueueStop(outAQ, TRUE);
    assert(theErr == noErr);
    
    AudioQueueFlush(inAQ);
    theErr = AudioQueueStop(inAQ, TRUE);
    assert(theErr == noErr);
    
    is_running = NO;
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
