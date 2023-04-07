//
//  SignalGenerator.m
//  MacSmoov
//
//  Created by Paul Zaremba on 4/7/23.
//

#import "SignalGenerator.h"

#define BUFFER_SIZE 2048

@implementation SignalGenerator {
    SignalGeneratorEngineWrapper* wrap;
    OSStatus theErr;
    AudioStreamBasicDescription asbd;
    AudioQueueRef outAQ;
    AudioQueueBufferRef buffers[4];
}

-(id) initWithSampleRate:(uint32_t)sample_rate numberOfChannels:(uint32_t)num_chans {
    self = [super init];
    if(self) {
        wrap = [[SignalGeneratorEngineWrapper alloc] initWithSampleRate:sample_rate numberOfChannels:num_chans];
        [self adjustVolume:0.50];
        
        memset(&asbd, 0, sizeof(asbd));
        
        asbd.mFormatID = kAudioFormatLinearPCM;
        asbd.mBitsPerChannel = 32;
        asbd.mSampleRate = 48000;
        asbd.mFramesPerPacket = 1;
        asbd.mChannelsPerFrame = 2;
        asbd.mBytesPerPacket = 8;
        asbd.mBytesPerFrame = 8;
        asbd.mFormatFlags = kLinearPCMFormatFlagIsFloat;
        
        //theErr = AudioQueueNewOutput(&asbd, audioOutQcb, (__bridge void*)self, CFRunLoopGetMain(), kCFRunLoopCommonModes, 0, &outAQ);
        theErr = AudioQueueNewOutput(&asbd, audioOutQcb, (__bridge void*)self, NULL, NULL, 0, &outAQ);
        
        assert(theErr == noErr);
        
        AudioQueueAllocateBuffer(outAQ, BUFFER_SIZE*sizeof(float), &buffers[0]);
        buffers[0]->mAudioDataByteSize = BUFFER_SIZE*sizeof(float);
        AudioQueueAllocateBuffer(outAQ, BUFFER_SIZE*sizeof(float), &buffers[1]);
        buffers[1]->mAudioDataByteSize = BUFFER_SIZE*sizeof(float);
        AudioQueueAllocateBuffer(outAQ, BUFFER_SIZE*sizeof(float), &buffers[2]);
        buffers[2]->mAudioDataByteSize = BUFFER_SIZE*sizeof(float);
        AudioQueueAllocateBuffer(outAQ, BUFFER_SIZE*sizeof(float), &buffers[3]);
        buffers[3]->mAudioDataByteSize = BUFFER_SIZE*sizeof(float);
    }
    
    return self;
}

-(void) configureWithType:(SIG_GEN_TYPE) type frequency:(uint32_t)frequency {
    [wrap configureWithType:type frequency:frequency];
}

-(void) adjustVolume:(float) volume {
    [wrap adjustVolume:volume];
}

void listenerCb(void *inUserData, AudioQueueRef inAQ, AudioQueuePropertyID inID) {
    NSLog(@"Listener CB called.");
}

void audioOutQcb(void *inUserData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer) {
    NSLog(@"audioOutQcb %p", inAQ);
    
    SignalGenerator* gen = (__bridge SignalGenerator*) inUserData;
    
    NSLog(@"Gen = %@", gen);
    
    float tmpBuf[BUFFER_SIZE];
    [gen->wrap getNextBuffer:tmpBuf ofSize:BUFFER_SIZE];
    
    memcpy(inBuffer->mAudioData, tmpBuf, BUFFER_SIZE*sizeof(float));
    
    AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, NULL);
}

-(void) startSignal {
    audioOutQcb((__bridge void*) self, outAQ, buffers[0]);
    audioOutQcb((__bridge void*) self, outAQ, buffers[1]);
    audioOutQcb((__bridge void*) self, outAQ, buffers[2]);
    audioOutQcb((__bridge void*) self, outAQ, buffers[3]);
    
    theErr = AudioQueueStart(outAQ, NULL);
    
    assert(theErr == noErr);
}

-(void) stopSignal {
    theErr = AudioQueueStop(outAQ, TRUE);
    assert(theErr == noErr);
    theErr = AudioQueueFlush(outAQ);
    assert(theErr == noErr);
}

@end
