//
//  AppDelegate.m
//  MacSmoov
//
//  Created by Paul Zaremba on 3/31/23.
//

#import "AppDelegate.h"
#import <AudioToolbox/AudioToolbox.h>
#import "SignalGenerator.h"
#import "AGCControlsView.h"
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
AGCControlsView* agc_controls_view;
MultibandControlsView* multiband_controls_view;
ProcessorCoreWrapper* proc_core_wrapper;
NSUserDefaults *prefs;
NSNumber* stored_output_device;
NSNumber* stored_input_device;

NSMutableArray* audio_devices_output;
NSMutableArray* audio_devices_input;

Boolean shutting_down = NO;

- (void) queryMeterLevels:(id)param {
    
    NSLog(@"Started meter driver thread.");
    
    float mainInLrms, mainInRrms, mainInLpeak, mainInRpeak;
    float stereoEnhanceLRDiff;
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
    
    float lookahead_limiter_gr;
    float clipper_action_l, clipper_action_r;
    
    float mainOutLrms, mainOutRrms, mainOutLpeak, mainOutRpeak;
    
    while(false == shutting_down) {
        usleep(33000);
        
        [proc_core_wrapper getMainInLevelsLrms:&mainInLrms Rrms:&mainInRrms Lpeak:&mainInLpeak Rpeak:&mainInRpeak];
        [proc_core_wrapper getStereoEnhanceLRDiff:&stereoEnhanceLRDiff];
        [proc_core_wrapper get2bandAGCGainReductionlo:&gainReduct2blo hi:&gainReduct2bhi gatelo:&gate_open_agc2_lo gatehi:&gate_open_agc2_hi];
        [proc_core_wrapper get5bandCompressorGainReduction:bands_gr limiters:bands_lim gates:bands_gate_open];
        [proc_core_wrapper getLookaheadLimiterGainReduction:&lookahead_limiter_gr];
        [proc_core_wrapper getClipperActionL:&clipper_action_l R:&clipper_action_r];
        [proc_core_wrapper getMainOutLevelsLrms:&mainOutLrms Rrms:&mainOutRrms Lpeak:&mainOutLpeak Rpeak:&mainOutRpeak];
        
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(void){
            //Background Thread
            dispatch_async(dispatch_get_main_queue(), ^(void){
                [self->_level_main_in set_levels_Lrms:mainInLrms Rrms:mainInRrms Lpeak:mainInLpeak Rpeak:mainInRpeak];
                [self->_stereo_enhance_lr_diff set_level:stereoEnhanceLRDiff];
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
                              
                [self->_lookahead_limiter_meter set_comp_lo:lookahead_limiter_gr hi:lookahead_limiter_gr];
                [self->_clipper_meter set_comp_lo:clipper_action_l hi:clipper_action_r];
                [self->_level_main_out set_levels_Lrms:mainOutLrms Rrms:mainOutRrms Lpeak:mainOutLpeak Rpeak:mainOutRpeak];
            });
        });
    }
    
    NSLog(@"Exiting meter driver thread.");
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    
    shutting_down = NO;
    
    [_level_main_in setMeterDbMinVal:-30.0f maxVal:1.0f];
    [_stereo_enhance_lr_diff setMeterLinearMinVal:0.0f maxVal:0.5f];
    [_stereo_enhance_lr_diff set_meter_colors_rms:[NSColor greenColor] peak:[NSColor greenColor]];
    [_comp_2band_agc set_meter_range:-24.0];
    [_comp_2band_agc set_meter_color:[NSColor magentaColor]];
    [_comp_5band set_meter_color:[NSColor magentaColor]];
    [_comp_5band set_meter_range:-12.0];
    [_lim_5band set_meter_color:[NSColor yellowColor]];
    [_lim_5band set_meter_range:-12.0];
    [_lookahead_limiter_meter set_meter_range:-10.0];
    [_lookahead_limiter_meter set_meter_color:[NSColor magentaColor]];
    [_clipper_meter set_meter_range: -0.1f];
    [_clipper_meter set_meter_color:[NSColor yellowColor]];
    
    //TODO:  initialize signal generators here
    //siggenvc = [[SignalGeneratorViewController alloc] init];
    //[self output_device_changed:@"AQDefaultOutput"];
    
    self.sysaudio = [[OSXAudioInterface alloc] init];
    
    audio_devices_input = [[NSMutableArray alloc] init];
    audio_devices_output = [[NSMutableArray alloc] init];
    [self.sysaudio get_all_input_devices:audio_devices_input];
    [self.sysaudio get_all_output_devices:audio_devices_output];
    
    NSLog(@"Input devices:\n");
    for(AudioDevice* d in audio_devices_input) {
        [d print_device];
    }
    
    NSLog(@"Output devices:\n");
    for(AudioDevice* d in audio_devices_output) {
        [d print_device];
    }
    
    [self.sysaudio set_input_device_from_name:@"Studio 26c"];
    [self.sysaudio set_output_device_from_name:@"Studio 26c"];
    
    audio_device_selector = [[AudioDeviceSelector alloc] initWithInputDevices:audio_devices_input outputDevices:audio_devices_output];
    [audio_device_selector set_watcher_for_output_device_change:self andSelector:@selector(output_device_changed:)];
    [audio_device_selector set_watcher_for_input_device_change:self andSelector:@selector(input_device_changed:)];
    
    //NSString* CONFIG_FILENAME = @"/Users/zaremba/Library/Containers/com.tonekids.osx.MacSmoov/Data/tmp/config.yml";
    NSString* BUNDLED_CONFIG_FILENAME = @"config.yml";
    NSBundle *bundle = [NSBundle mainBundle];
    NSString *resourceFolderPath = [bundle resourcePath];
    NSString* BUNDLED_CONFIG_FILEPATH = [NSString stringWithFormat:@"%@/%@", resourceFolderPath, BUNDLED_CONFIG_FILENAME];
    
    NSString *executableName = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleExecutable"];
    NSArray<NSURL *>* app_support_paths = [[NSFileManager defaultManager] URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask];
    
    NSString* app_support_path = [[app_support_paths objectAtIndex:0] path];
    app_support_path = [app_support_path stringByAppendingPathComponent:executableName];
    
    NSLog(@"Creating app support directory if it doesn't exist...");
        
    NSError * error = nil;
    [[NSFileManager defaultManager] createDirectoryAtPath:app_support_path withIntermediateDirectories:YES attributes:nil error:&error];
    if (error != nil) {
        NSLog(@"error creating app support directory directory: %@", error);
        exit(-1);
    }
    
    NSString* config_file_path = [app_support_path stringByAppendingPathComponent:BUNDLED_CONFIG_FILENAME];
    
    /*
     * Check to see if there is a config file there already.  If it doesn't exist, copy the factory defaults from
     * inside the bundle.
     */
    
    bool config_file_exists = [[NSFileManager defaultManager] fileExistsAtPath:config_file_path];
    
    if(!config_file_exists) {
        NSLog(@"No config file found.  Copying factory defaults from bundle...");
        [[NSFileManager defaultManager] copyItemAtPath:BUNDLED_CONFIG_FILEPATH toPath:config_file_path error:&error];
        
        if(error != nil) {
            NSLog(@"Error copying factory defaults to app support directory: %@", error);
            exit(-2);
        }
    }
    
    proc_core_wrapper = [[ProcessorCoreWrapper alloc] initWithSampleRate:self.sysaudio.sample_rate numberOfChannels:self.sysaudio.num_channels bufferSize:(uint32_t)self.sysaudio.buffer_size configFilename:config_file_path];
    
    
    float input_gain_L, input_gain_R;
    [proc_core_wrapper getMainInGainDBL:&input_gain_L R:&input_gain_R];
    [_slider_gain_main_in setIntValue:input_gain_L];
    
    
    bool se_enabled = [proc_core_wrapper getStereoEnhanceEnabled];
    float se_drive = [proc_core_wrapper getStereoEnhanceDrive];
    int se_slider_val = ((se_drive * 100.0f) - 100.0f);
    
    if(se_enabled) {
        [_stereo_enhance_enabled setState:NSControlStateValueOn];
    } else {
        [_stereo_enhance_enabled setState:NSControlStateValueOff];
    }
    
    [_slider_stereo_enhance setIntValue:se_slider_val];
    
    AGC_PARAMS agc_settings;
    [proc_core_wrapper get_agc_settings:&agc_settings];
    agc_controls_view = [[AGCControlsView alloc] initWithSettings:agc_settings delegate:self];
    
    MULTIBAND_PARAMS mb_settings;
    [proc_core_wrapper get_multiband_settings:&mb_settings];
    multiband_controls_view = [[MultibandControlsView alloc] initWithSettings:mb_settings delegate:self];
    
    float output_gain_L, output_gain_R;
    [proc_core_wrapper getMainOutGainDBL:&output_gain_L R:&output_gain_R];
    [_slider_gain_main_out setIntValue:output_gain_L];
        
    [self.sysaudio set_processor_hook:[proc_core_wrapper get_proc_core_hook]];
    
    if(NO == [proc_core_wrapper prepare]) {
        NSLog(@"PROCESSOR CORE IS NOT READY.  WILL NOT START.");
        return;
    }
    
    [self.sysaudio start];
    
    [NSThread detachNewThreadSelector:@selector(queryMeterLevels:) toTarget:self withObject:nil];
    
    /* To ease view controller debugging hell */
    [self agcFactoryMenuSelected:self];
    [self multibandAdjustMenuSelected:self];
}

- (NSApplicationTerminateReply) applicationShouldTerminate:(NSApplication *) sender {
    NSLog(@"Got a teminate!");
    return NSTerminateNow;
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

-(void) output_device_changed:(AudioDevice*)output_device {
    NSLog(@"Got notification from AudioDeviceSelector about new output device: %@", output_device);
    if(nil != output_device) {
 //       [prefs setObject:output_device forKey:@"OUTPUT_DEVICE"];
 //       AudioDevice* sel_outdev = [audio_devices_output objectForKey:output_device];
        [self.sysaudio stop];
        [self.sysaudio set_output_device:output_device];
        [self.sysaudio start];
        //[siggenvc outputDeviceChanged:output_device];
        //[process_sys_iface outputDeviceChanged:output_device];
    }
    else {
        NSLog(@"WARNING: Got null output_device notification.");
    }
}

-(void) input_device_changed:(AudioDevice*)input_device {
    NSLog(@"Got notification from AudioDeviceSelector about new input device: %@", input_device);
    if(nil != input_device) {
//        [prefs setObject:input_device forKey:@"INPUT_DEVICE"];
//        AudioDevice* sel_indev = [audio_devices_input objectForKey:input_device];
        [self.sysaudio stop];
        [self.sysaudio set_input_device:input_device];
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
}

-(IBAction) masterBypassSelected:(id)sender {
    NSButton* button_bypass = (NSButton*) sender;
    
    if(button_bypass.state == NSControlStateValueOn) {
        NSLog(@"Master Bypass On");
    }
    else {
        NSLog(@"Master Bypass Off");
    }
    
    [proc_core_wrapper setMasterBypass:button_bypass.state];
}

-(IBAction) agcFactoryMenuSelected:(id)sender {
    //[agc_controls_view showPanel];
}

-(IBAction) multibandAdjustMenuSelected:(id)sender {
    //[multiband_controls_view showPanel];
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
    NSEvent *event = [[NSApplication sharedApplication] currentEvent];
    BOOL mouseUp = event.type == NSEventTypeLeftMouseUp;
    
    if(mouseUp) {
        [proc_core_wrapper mainInGainChangeDoneL:gmi.cell.floatValue R:gmi.cell.floatValue];
    }
}

-(IBAction) stereoEnhanceEnableChanged:(id)sender {
    NSButton* se_enabled = sender;
    [proc_core_wrapper setStereoEnhanceEnabled: se_enabled.state];
}

-(IBAction) stereoEnhanceSliderChanged:(id)sender {
    /*
     * The slider, as configured, goes from 1 to 200.  I'm mapping that
     * to a stereo enhance drive (L-R boost) from 1.00 to 3.00.
     */
    NSSlider* slider = (NSSlider*)sender;
    float se_drive = 1.00f + ([slider intValue] / 100.0f);
    [proc_core_wrapper setStereoEnhanceDrive:se_drive];
}

-(void) agc_params_changed:(AGC_PARAMS) params {
    NSLog(@"agc_params_changed delegate called on app delegate!");
    [proc_core_wrapper change_agc_settings:params];
}

-(void) multiband_params_changed:(MULTIBAND_PARAMS) params {
    NSLog(@"multiband_params_changed delegate called on app delegate!");
    [proc_core_wrapper change_multiband_settings:params];
}

-(void) band_enablement_changed:(NSControlStateValue[]) band_states {
    NSLog(@"band_enablement_changed delegate called on app delegate!");
    [proc_core_wrapper setBandEnablement:band_states];
}

-(IBAction) adjustGainMainOut:(id)sender {
    NSSlider* gmi = sender;
    //NSLog(@"GainMainOut value changed: %4.2f dB", gmo.cell.floatValue);
    [proc_core_wrapper setMainOutGainDBL:gmi.cell.floatValue R:gmi.cell.floatValue];
    NSEvent *event = [[NSApplication sharedApplication] currentEvent];
    BOOL mouseUp = event.type == NSEventTypeLeftMouseUp;
    
    if(mouseUp) {
        [proc_core_wrapper mainOutGainChangeDoneL:gmi.cell.floatValue R:gmi.cell.floatValue];
    }
}

-(void) setup_ui_from_defaults {
    
}

@end
