//
//  AppDelegate.m
//  MacSmoov
//
//  Created by Paul Zaremba on 3/31/23.
//

#import "AppDelegate.h"
#import <AudioToolbox/AudioToolbox.h>
#import "SignalGenerator.h"
#import "MultibandControlsView.h"
#import "ProcessorCoreWrapper.h"
#import "OSXAudioInterface.h"

@interface AppDelegate () {
    
}

-(void) setup_ui_from_defaults;

@property (strong) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

SignalGeneratorViewController *siggenvc;
AudioDeviceSelector *audio_device_selector;
MultibandControlsView* multiband_controls_view;
ProcessorCoreWrapper* proc_core_wrapper;
NSUserDefaults *prefs;
NSNumber* stored_output_device;
NSNumber* stored_input_device;

NSMutableDictionary* audio_devices_output;
NSMutableDictionary* audio_devices_input;

Boolean shutting_down;

- (void) queryMeterLevels:(id)param {
    
    NSLog(@"Started meter driver thread.");
    
    float mainInLrms, mainInRrms, mainInLpeak, mainInRpeak;
    float gainReduct2blo, gainReduct2bhi;
    bool gate_open_agc2_lo, gate_open_agc2_hi;
    float** bands_gr = (float**)malloc(5 * sizeof(float*));
    bands_gr[0] = malloc(sizeof(float));
    bands_gr[1] = malloc(sizeof(float));
    bands_gr[2] = malloc(sizeof(float));
    bands_gr[3] = malloc(sizeof(float));
    bands_gr[4] = malloc(sizeof(float));
    
    float** bands_lim = (float**)malloc(5 * sizeof(float*));
    bands_lim[0] = malloc(sizeof(float));
    bands_lim[1] = malloc(sizeof(float));
    bands_lim[2] = malloc(sizeof(float));
    bands_lim[3] = malloc(sizeof(float));
    bands_lim[4] = malloc(sizeof(float));
    
    bool** bands_gate_open = (bool**)malloc(5 * sizeof(bool*));
    bands_gate_open[0] = malloc(sizeof(bool));
    bands_gate_open[1] = malloc(sizeof(bool));
    bands_gate_open[2] = malloc(sizeof(bool));
    bands_gate_open[3] = malloc(sizeof(bool));
    bands_gate_open[4] = malloc(sizeof(bool));
    
    while(false == shutting_down) {
        [proc_core_wrapper getMainInLevelsLrms:&mainInLrms Rrms:&mainInRrms Lpeak:&mainInLpeak Rpeak:&mainInRpeak];
        [proc_core_wrapper get2bandAGCGainReductionlo:&gainReduct2blo hi:&gainReduct2bhi gatelo:&gate_open_agc2_lo gatehi:&gate_open_agc2_hi];
        [proc_core_wrapper get5bandCompressorGainReduction:bands_gr limiters:bands_lim gates:bands_gate_open];
        
        dispatch_async(dispatch_get_global_queue( DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(void){
            //Background Thread
            dispatch_async(dispatch_get_main_queue(), ^(void){
                [self->_level_main_in set_levels_Lrms:mainInLrms Rrms:mainInRrms Lpeak:mainInLpeak Rpeak:mainInRpeak];
                [self->_comp_2band_agc set_comp_lo:gainReduct2blo hi:gainReduct2bhi];
                if(!gate_open_agc2_lo) {
                    [self->_agc_lo_gate_closed setFillColor:[NSColor redColor]];
                }
                else {
                    [self->_agc_lo_gate_closed setFillColor:[NSColor blackColor]];
                }
                if(!gate_open_agc2_hi) {
                    [self->_agc_hi_gate_closed setFillColor:[NSColor redColor]];
                }
                else {
                    [self->_agc_hi_gate_closed setFillColor:[NSColor blackColor]];
                }
                [self->_comp_5band set_comps:bands_gr];
                [self->_lim_5band set_comps:bands_lim];
                
                if(!*(bands_gate_open[0])) {
                    [self->_gate_b1 setFillColor:[NSColor redColor]];
                }
                else {
                    [self->_gate_b1 setFillColor:[NSColor blackColor]];
                }
                if(!*(bands_gate_open[1])) {
                    [self->_gate_b2 setFillColor:[NSColor redColor]];
                }
                else {
                    [self->_gate_b2 setFillColor:[NSColor blackColor]];
                }
                if(!*(bands_gate_open[2])) {
                    [self->_gate_b3 setFillColor:[NSColor redColor]];
                }
                else {
                    [self->_gate_b3 setFillColor:[NSColor blackColor]];
                }
                if(!*(bands_gate_open[3])) {
                    [self->_gate_b4 setFillColor:[NSColor redColor]];
                }
                else {
                    [self->_gate_b4 setFillColor:[NSColor blackColor]];
                }
                if(!*(bands_gate_open[4])) {
                    [self->_gate_b5 setFillColor:[NSColor redColor]];
                }
                else {
                    [self->_gate_b5 setFillColor:[NSColor blackColor]];
                }
            });
        });
        
        usleep(70000);
    }
    
    NSLog(@"Exiting meter driver thread.");
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    OSStatus err = 0;

    shutting_down = NO;
    
    [_comp_2band_agc set_meter_range:-35.0];
    [_comp_5band set_meter_color:[NSColor magentaColor]];
    [_comp_5band set_meter_range:-10.0];
    [_lim_5band set_meter_color:[NSColor yellowColor]];
    [_lim_5band set_meter_range:-1.5];
    
    
    NSUserDefaultsController *prefs_controller = [NSUserDefaultsController sharedUserDefaultsController];
    prefs = prefs_controller.defaults;
    
    [_slider_gain_main_in setFloatValue:[[prefs objectForKey:@"GAIN_IN_MAIN"] floatValue]];
    
    //TODO:  initialize signal generators here, and the eventual audio passthrough/processing chain
    siggenvc = [[SignalGeneratorViewController alloc] init];
    //[self output_device_changed:@"AQDefaultOutput"];
    
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
    
    audio_device_selector = [[AudioDeviceSelector alloc] initWithInputDevices:(NSMutableDictionary*)audio_devices_input outputDevices:(NSMutableDictionary*)audio_devices_output];
    [audio_device_selector set_watcher_for_output_device_change:self andSelector:@selector(output_device_changed:)];
    [audio_device_selector set_watcher_for_input_device_change:self andSelector:@selector(input_device_changed:)];
    
    //process_sys_iface = [[ProcessorSysInterface alloc] initWithSampleRate:self.sysaudio.sample_rate numberOfChannels:self.sysaudio.num_channels bufferSize:self.sysaudio.buffer_size];
    proc_core_wrapper = [[ProcessorCoreWrapper alloc] initWithSampleRate:self.sysaudio.sample_rate numberOfChannels:self.sysaudio.num_channels bufferSize:self.sysaudio.buffer_size];
    
    multiband_controls_view = [[MultibandControlsView alloc] initWithPrefs:prefs delegate:self];
        
    [self.sysaudio set_processor_hook:[proc_core_wrapper get_proc_core_hook]];
    [self.sysaudio start];
    
    [NSThread detachNewThreadSelector:@selector(queryMeterLevels:) toTarget:self withObject:nil];

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
    shutting_down = YES;
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
        [self.sysaudio stop];
        [self.sysaudio set_output_device:sel_outdev];
        [self.sysaudio start];
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
        [self.sysaudio stop];
        [self.sysaudio set_input_device:sel_indev];
        [self.sysaudio start];
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

-(IBAction) multibandAdjustMenuSelected:(id)sender {
    [multiband_controls_view showPanel];
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


-(IBAction) adjustGainMainIn:(id)sender {
    NSSlider* gmi = sender;
    //NSLog(@"GainMainIn value changed: %4.2f dB", gmo.cell.floatValue);
    [proc_core_wrapper setMainInGainDBL:gmi.cell.floatValue R:gmi.cell.floatValue];
    [prefs setObject:[NSString stringWithFormat:@"%f", gmi.cell.floatValue] forKey:@"GAIN_IN_MAIN"];
}

-(void) multiband_params_changed:(MULTIBAND_PARAMS) params {
    NSLog(@"multiband_params_changed delegate called on app delegate!");
    [proc_core_wrapper change_multiband_settings:params];
}

-(void) band_enablement_changed:(NSControlStateValue[]) band_states {
    NSLog(@"band_enablement_changed delegate called on app delegate!");
    [proc_core_wrapper setBandEnablement:band_states];
}

-(void) setup_ui_from_defaults {
    
}

@end
