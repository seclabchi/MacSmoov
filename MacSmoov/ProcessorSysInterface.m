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

#define BUFFER_SIZE 2048

@interface ProcessorSysInterface() {
    OSStatus theErr;
    AudioStreamBasicDescription asbd;
    AudioQueueRef inAQ;
    AudioQueueRef outAQ;
    AudioQueueBufferRef in_buffer;
    AudioQueueBufferRef out_buffers[5];
    AudioQueueBufferRef cur_out_buf;
    uint32_t cur_out_buf_index;
    BOOL is_running;
    NSLock* lock_inout;
}

-(void) restart;
@end

@implementation ProcessorSysInterface

uint8_t inprocbufnum = 0;
uint8_t outprocbufnum = 0;

void audioprocOutQcb(void *inUserData, AudioQueueRef outAQ, AudioQueueBufferRef outBuffer) {
    ProcessorSysInterface* psi = (__bridge ProcessorSysInterface*) inUserData;
    //NSLog(@"Got capture buffer!");
    
}

void audioprocInQcb(void *inUserData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer, const AudioTimeStamp* inStartTime, UInt32 npd, const AudioStreamPacketDescription* aspd) {
    
    ProcessorSysInterface* psi = (__bridge ProcessorSysInterface*) inUserData;
    
    if(psi->is_running) {
        psi->out_buffers[psi->cur_out_buf_index]->mAudioDataByteSize = inBuffer->mAudioDataByteSize;
        [psi->core processWithInput:inBuffer->mAudioData output:psi->out_buffers[psi->cur_out_buf_index]->mAudioData ofSize:inBuffer->mAudioDataByteSize/sizeof(float)];
        //memcpy(psi->out_buffers[psi->cur_out_buf_index]->mAudioData, inBuffer->mAudioData, inBuffer->mAudioDataByteSize);
        AudioQueueEnqueueBuffer(psi->outAQ, psi->out_buffers[psi->cur_out_buf_index], 0, nil);
        
        if(psi->cur_out_buf_index == 0) {
            psi->cur_out_buf_index = 1;
        }
        else if(psi->cur_out_buf_index == 1) {
            psi->cur_out_buf_index = 0;
        }
        else if(psi->cur_out_buf_index == 2) {
            psi->cur_out_buf_index = 3;
        }
        else if(psi->cur_out_buf_index == 3) {
            psi->cur_out_buf_index = 4;
        }
        else {
            psi->cur_out_buf_index = 0;
        }
        
        //NSLog(@"cur_out_buf_index = %d", psi->cur_out_buf_index);
        
        AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, NULL);
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
        
        core = [[ProcessorCoreWrapper alloc] initWithSampleRate:48000 numberOfChannels:2];

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
        
        theErr = AudioQueueAllocateBuffer(inAQ, BUFFER_SIZE*sizeof(float), &in_buffer);
        assert(theErr == noErr);
        in_buffer->mAudioDataByteSize = BUFFER_SIZE*sizeof(float);
        
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
        
        theErr = AudioQueueAllocateBuffer(outAQ, BUFFER_SIZE*sizeof(float), &out_buffers[4]);
        assert(theErr == noErr);
        out_buffers[4]->mAudioDataByteSize = BUFFER_SIZE*sizeof(float);
        
        cur_out_buf_index = 0;

    }
    
    return self;
}

-(void) start {
        
    CFStringRef outdev_cfstr = (__bridge CFStringRef)output_device;
    theErr = AudioQueueSetProperty(outAQ, kAudioQueueProperty_CurrentDevice, &outdev_cfstr, sizeof(&outdev_cfstr));
    assert(theErr == noErr);

    theErr = AudioQueueStart(outAQ, NULL);
    assert(theErr == noErr);
    
    CFStringRef indev_cfstr = (__bridge CFStringRef)input_device;
    theErr = AudioQueueSetProperty(inAQ, kAudioQueueProperty_CurrentDevice, &indev_cfstr, sizeof(&indev_cfstr));
    assert(theErr == noErr);
    
    
    theErr = AudioQueueStart(inAQ, NULL);
    assert(theErr == noErr);
    
    theErr = AudioQueueEnqueueBuffer(inAQ, in_buffer, 0, NULL);
    assert(theErr == noErr);
    
    is_running = YES;
}

-(void) stop {
    //AudioQueueFlush(outAQ);
    is_running = NO;
    
    //AudioQueueFlush(inAQ);
    theErr = AudioQueueStop(inAQ, TRUE);
    assert(theErr == noErr);
    
    theErr = AudioQueueStop(outAQ, TRUE);
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
