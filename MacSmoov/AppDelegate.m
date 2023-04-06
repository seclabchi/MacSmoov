//
//  AppDelegate.m
//  MacSmoov
//
//  Created by Paul Zaremba on 3/31/23.
//

#import "AppDelegate.h"
#import <AudioToolbox/AudioToolbox.h>
#import "SignalGeneratorWrapper.h"

@interface AppDelegate ()

@property (strong) IBOutlet NSWindow *window;

@property (strong) IBOutlet NSSlider *sliderMasterVolume;
@end

@implementation AppDelegate

#define BUFFER_SIZE 8192

SignalGenerator* sig_gen = NULL;
OSStatus theErr;
AudioStreamBasicDescription asbd = {0};
AudioQueueRef outAQ;

AudioQueueBufferRef buffers[2];

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    //TODO:  initialize signal generators here, and the eventual audio passthrough/processing chain
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


- (BOOL)applicationSupportsSecureRestorableState:(NSApplication *)app {
    return YES;
}

- (void)windowWillClose:(NSNotification*) notification {
    NSLog(@"windowWillClose notification object %@", notification.object);
    
    [NSApp terminate:self];
}

- (IBAction)openDocument:(id)sender {
    // Create the File Open Dialog class.
    NSOpenPanel* openDlg = [NSOpenPanel openPanel];

    // Enable the selection of files in the dialog.
    openDlg.canChooseFiles = YES;

    // Enable the selection of directories in the dialog.
    openDlg.canChooseDirectories = NO;
    
    openDlg.allowsMultipleSelection = NO;

    // Display the dialog.  If the OK button was pressed,
    // process the files.
    if ( [openDlg runModal] == NSFileHandlingPanelOKButton )
    {
        NSURL* fileUrl = [openDlg URL];
        NSLog(@"File %@ selected.", fileUrl.absoluteString);
        // Do something with the filename.
        [self openAudioFile:fileUrl];
    }
    
}

-(void) openAudioFile:(NSURL*)fileUrl {
    AudioFileID audioFile;
    OSStatus theErr = noErr;
    theErr = AudioFileOpenURL((__bridge CFURLRef)fileUrl,
                              kAudioFileReadPermission,
                              0,
                              &audioFile);
    assert(theErr == noErr);
    
    UInt32 dictionarySize = 0;
    theErr = AudioFileGetPropertyInfo(audioFile,
                                      kAudioFilePropertyInfoDictionary,
                                      &dictionarySize,
                                      0);
    assert(theErr == noErr);
    
    CFDictionaryRef dictionary;
    theErr = AudioFileGetProperty(audioFile,
                                  kAudioFilePropertyInfoDictionary,
                                  &dictionarySize,
                                  &dictionary);
    assert(theErr == noErr);
    
    NSLog(@"Opened audio file with property dictionary %@:", dictionary);
    
    CFRelease(dictionary);
    theErr = AudioFileClose(audioFile);
    assert(theErr == noErr);
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

-(IBAction) adjustVolume:(id)sender {
    float volume = [_sliderMasterVolume floatValue];
    [sig_gen adjustVolume:volume];
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

-(IBAction) generatePinkNoise(id)sender {
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
