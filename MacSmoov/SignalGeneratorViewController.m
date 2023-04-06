//
//  SignalGeneratorViewController.m
//  MacSmoov
//
//  Created by Paul Zaremba on 4/2/23.
//

#import <AudioToolbox/AudioToolbox.h>

#import "SignalGeneratorViewController.h"

@interface SignalGeneratorViewController ()
@property (strong) IBOutlet NSPanel *sig_gen_panel;
@end


@implementation SignalGeneratorViewController

SignalGenerator* sig_gen;
OSStatus theErr;
AudioStreamBasicDescription asbd = {0};
AudioQueueRef outAQ;

AudioQueueBufferRef buffers[2];

-(void)showPanel {
    NSPanel* panel = (NSPanel*)[[self view] window];
    [self showSignalGenerators:nil];
    NSLog(@"%@", panel);
}

-(IBAction) showSignalGenerators:(id)sender {
    [_sig_gen_panel setFloatingPanel:YES];
    [_sig_gen_panel orderFront:nil];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    NSLog(@"Sig gen viewDidLoad");
    // Do view setup here.
    [sig_gen initWithSampleRate:48000 numberOfChannels:2];
}

-(void)viewWillDisappear {
    
}

-(IBAction) functionTypeGen1:(id)sender {
    
}

-(IBAction) functionTypeGen2:(id)sender {
    
}

-(IBAction) functionTypeGen3:(id)sender {
    
}

-(IBAction) functionTypeGen4:(id)sender {
    
}


void listenerCb(void *inUserData, AudioQueueRef inAQ, AudioQueuePropertyID inID) {
    NSLog(@"Listener CB called.");
}

void audioOutQcb(void *inUserData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer) {
    //NSLog(@"Got AQ callback");
    
    float tmpBuf[BUFFER_SIZE];
    [sig_gen getNextBuffer:tmpBuf ofSize:BUFFER_SIZE];
    
    memcpy(inBuffer->mAudioData, tmpBuf, BUFFER_SIZE*sizeof(float));
    
    AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, NULL);
}

-(IBAction) generateSine400Hz:(id)sender {
    
    sig_gen = [[SignalGenerator alloc] initWithSampleRate:48000 numberOfChannels:2];
    [sig_gen configureWithType:SINE frequency:440];
    
    asbd.mFormatID = kAudioFormatLinearPCM;
    asbd.mBitsPerChannel = 32;
    asbd.mSampleRate = 48000;
    asbd.mFramesPerPacket = 1;
    asbd.mChannelsPerFrame = 2;
    asbd.mBytesPerPacket = 8;
    asbd.mBytesPerFrame = 8;
    asbd.mFormatFlags = kLinearPCMFormatFlagIsFloat;
    
    theErr = AudioQueueNewOutput(&asbd, audioOutQcb, NULL, CFRunLoopGetMain(), kCFRunLoopCommonModes, 0, &outAQ);
    
    assert(theErr == noErr);
    
    AudioQueueAllocateBuffer(outAQ, BUFFER_SIZE*sizeof(float), &buffers[0]);
    buffers[0]->mAudioDataByteSize = BUFFER_SIZE*sizeof(float);
    
    audioOutQcb(NULL, outAQ, buffers[0]);
    
    AudioQueueAllocateBuffer(outAQ, BUFFER_SIZE*sizeof(float), &buffers[1]);
    buffers[1]->mAudioDataByteSize = BUFFER_SIZE*sizeof(float);
    
    audioOutQcb(NULL, outAQ, buffers[1]);
    
    theErr = AudioQueueStart(outAQ, NULL);
    
    assert(theErr == noErr);
    
}

-(IBAction) generateSine1kHz:(id)sender {
    
    sig_gen = [[SignalGenerator alloc] initWithSampleRate:48000 numberOfChannels:2];
    [sig_gen configureWithType:SINE frequency:1000];
    
    asbd.mFormatID = kAudioFormatLinearPCM;
    asbd.mBitsPerChannel = 32;
    asbd.mSampleRate = 48000;
    asbd.mFramesPerPacket = 1;
    asbd.mChannelsPerFrame = 2;
    asbd.mBytesPerPacket = 8;
    asbd.mBytesPerFrame = 8;
    asbd.mFormatFlags = kLinearPCMFormatFlagIsFloat;
    
    theErr = AudioQueueNewOutput(&asbd, audioOutQcb, NULL, CFRunLoopGetMain(), kCFRunLoopCommonModes, 0, &outAQ);
    
    assert(theErr == noErr);
    
    AudioQueueAllocateBuffer(outAQ, BUFFER_SIZE*sizeof(float), &buffers[0]);
    buffers[0]->mAudioDataByteSize = BUFFER_SIZE*sizeof(float);
    
    audioOutQcb(NULL, outAQ, buffers[0]);
    
    AudioQueueAllocateBuffer(outAQ, BUFFER_SIZE*sizeof(float), &buffers[1]);
    buffers[1]->mAudioDataByteSize = BUFFER_SIZE*sizeof(float);
    
    audioOutQcb(NULL, outAQ, buffers[1]);
    
    theErr = AudioQueueStart(outAQ, NULL);
    
    assert(theErr == noErr);
    
}

-(IBAction) generatePinkNoise:(id)sender {
    sig_gen = [[SignalGenerator alloc] initWithSampleRate:48000 numberOfChannels:2];
    [sig_gen configureWithType:SINE frequency:1000];
    
    asbd.mFormatID = kAudioFormatLinearPCM;
    asbd.mBitsPerChannel = 32;
    asbd.mSampleRate = 48000;
    asbd.mFramesPerPacket = 1;
    asbd.mChannelsPerFrame = 2;
    asbd.mBytesPerPacket = 8;
    asbd.mBytesPerFrame = 8;
    asbd.mFormatFlags = kLinearPCMFormatFlagIsFloat;
    
    theErr = AudioQueueNewOutput(&asbd, audioOutQcb, NULL, CFRunLoopGetMain(), kCFRunLoopCommonModes, 0, &outAQ);
    
    assert(theErr == noErr);
    
    AudioQueueAllocateBuffer(outAQ, BUFFER_SIZE*sizeof(float), &buffers[0]);
    buffers[0]->mAudioDataByteSize = BUFFER_SIZE*sizeof(float);
    
    audioOutQcb(NULL, outAQ, buffers[0]);
    
    AudioQueueAllocateBuffer(outAQ, BUFFER_SIZE*sizeof(float), &buffers[1]);
    buffers[1]->mAudioDataByteSize = BUFFER_SIZE*sizeof(float);
    
    audioOutQcb(NULL, outAQ, buffers[1]);
    
    theErr = AudioQueueStart(outAQ, NULL);
    
    assert(theErr == noErr);
}

-(IBAction) stopAudioGen:(id)sender {
    theErr = AudioQueueStop(outAQ, TRUE);
    
    assert(theErr == noErr);
}
@end
