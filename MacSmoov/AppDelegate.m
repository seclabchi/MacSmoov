//
//  AppDelegate.m
//  MacSmoov
//
//  Created by Paul Zaremba on 3/31/23.
//

#import "AppDelegate.h"
#import <AudioToolbox/AudioToolbox.h>
#import "SignalGenerator.h"
#import "ProcessorSysInterface.h"
#import "OSXAudioInterface.h"

@interface AppDelegate ()

@property (strong) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

SignalGeneratorViewController *siggenvc;
AudioDeviceSelector *audio_device_selector;
//ProcessorSysInterface* process_sys_iface;
NSUserDefaults *prefs;
NSNumber* stored_output_device;
NSNumber* stored_input_device;

NSMutableDictionary* audio_devices_output;
NSMutableDictionary* audio_devices_input;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    OSStatus err = 0;

    
    NSUserDefaultsController *prefs_controller = [NSUserDefaultsController sharedUserDefaultsController];
    prefs = prefs_controller.defaults;
    
       
    self.sysaudio = [[OSXAudioInterface alloc] initWithCurrentInputDevice:nil OutputDevice:nil];
    [self.sysaudio discoverDevices];
    
    audio_devices_output = self.sysaudio.output_devices;
    audio_devices_input = self.sysaudio.input_devices;
    
    stored_output_device = [prefs objectForKey:@"OUTPUT_DEVICE"];
    if(nil != stored_output_device) {
        AudioDevice* sel_outdev = [audio_devices_output objectForKey:stored_output_device];
        [self.sysaudio set_output_device:sel_outdev];
    }
    
    stored_input_device = [prefs objectForKey:@"INPUT_DEVICE"];
    if(nil != stored_input_device) {
        AudioDevice* sel_indev = [audio_devices_input objectForKey:stored_input_device];
        [self.sysaudio set_input_device:sel_indev];
    }
    
    [self.sysaudio go];
    
    //TODO:  initialize signal generators here, and the eventual audio passthrough/processing chain
    siggenvc = [[SignalGeneratorViewController alloc] init];
    //[self output_device_changed:@"AQDefaultOutput"];
    audio_device_selector = [[AudioDeviceSelector alloc] initWithInputDevices:(NSMutableDictionary*)audio_devices_input outputDevices:(NSMutableDictionary*)audio_devices_output];
    [audio_device_selector set_watcher_for_output_device_change:self andSelector:@selector(output_device_changed:)];
    [audio_device_selector set_watcher_for_input_device_change:self andSelector:@selector(input_device_changed:)];
    //process_sys_iface = [[ProcessorSysInterface alloc] init];
    //[process_sys_iface outputDeviceChanged:@"AppleUSBAudioEngine:Plantronics:Plantronics Blackwire 3210 Series:FFE5F399D3F84D558CDC32EA0790A041:2"];
    //[process_sys_iface start];
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

-(void) output_device_changed:(NSNumber*)output_device {
    NSLog(@"Got notification from AudioDeviceSelector about new output device: %@", output_device);
    if(nil != output_device) {
        [prefs setObject:output_device forKey:@"OUTPUT_DEVICE"];
        AudioDevice* sel_outdev = [audio_devices_output objectForKey:output_device];
        [self.sysaudio set_output_device:sel_outdev];
        //[siggenvc outputDeviceChanged:output_device];
        //[process_sys_iface outputDeviceChanged:output_device];
    }
    else {
        NSLog(@"WARNING: Got null output_device notification.");
    }
}

-(void) input_device_changed:(NSNumber*)input_device {
    NSLog(@"Got notification from AudioDeviceSelector about new input device: %@", input_device);
    if(nil != input_device) {
        [prefs setObject:input_device forKey:@"INPUT_DEVICE"];
        AudioDevice* sel_indev = [audio_devices_input objectForKey:input_device];
        [self.sysaudio set_input_device:sel_indev];
        //[process_sys_iface inputDeviceChanged:input_device];
    }
    else {
        NSLog(@"WARNING: Got null input_device notification.");
    }
}

- (IBAction) showSignalGenerators:(id)sender {
    [siggenvc showPanel];
}

-(IBAction) showAudioDeviceSelector:(id)sender {
    [audio_device_selector showPanel];
    [audio_device_selector scanDevices];
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


-(IBAction) adjustGainMainOut:(id)sender {
    
}



@end
