//
//  AppDelegate.h
//  MacSmoov
//
//  Created by Paul Zaremba on 3/31/23.
//

#import <Cocoa/Cocoa.h>
#import "SignalGeneratorViewController.h"
#import "AudioDeviceSelector.h"
#import "AGCControlsView.h"
#import "MultibandControlsView.h"
#import "LevelMeter.h"
#import "CompressionMeter.h"
#import "CompressionMeter5band.h"
#import "OSXAudioInterface.h"

@interface AppDelegate : NSObject <NSApplicationDelegate, MultibandControlsDelegate, AGCControlsDelegate>

@property (strong) IBOutlet LevelMeter* level_main_in;
@property (strong) IBOutlet NSSlider* slider_gain_main_in;
@property (strong) IBOutlet CompressionMeter* comp_2band_agc;
@property (strong) IBOutlet NSBox* agc_lo_gate_closed;
@property (strong) IBOutlet NSBox* agc_hi_gate_closed;
@property (strong) IBOutlet CompressionMeter5band* comp_5band;

@property (strong) IBOutlet NSBox* gate_b1;
@property (strong) IBOutlet NSBox* gate_b2;
@property (strong) IBOutlet NSBox* gate_b3;
@property (strong) IBOutlet NSBox* gate_b4;
@property (strong) IBOutlet NSBox* gate_b5;
@property (strong) IBOutlet CompressionMeter5band* lim_5band;

@property (nonatomic, strong) OSXAudioInterface * sysaudio;

-(void) output_device_changed:(NSNumber*)output_device;
-(void) input_device_changed:(NSNumber*)input_device;
-(IBAction) showSignalGenerators:(id)sender;
-(IBAction) showAudioDeviceSelector:(id)sender;

-(IBAction) masterBypassSelected:(id)sender;

-(IBAction) adjustGainMainIn:(id)sender;

-(IBAction) agcFactoryMenuSelected:(id)sender;
-(IBAction) multibandAdjustMenuSelected:(id)sender;

-(void) queryMeterLevels:(id)param;
-(void) openAudioFile:(NSURL*)fileUrl;

-(void) agc_params_changed:(AGC_PARAMS) params;
-(void) multiband_params_changed:(MULTIBAND_PARAMS) params;
-(void) band_enablement_changed:(NSControlStateValue[]) band_states;

@end

