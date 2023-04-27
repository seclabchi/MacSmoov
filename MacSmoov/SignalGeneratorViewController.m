//
//  SignalGeneratorViewController.m
//  MacSmoov
//
//  Created by Paul Zaremba on 4/2/23.
//


#import "SignalGeneratorViewController.h"
#import "SignalGenerator.h"

@interface SignalGeneratorViewController ()
@property (strong) IBOutlet NSPanel *sig_gen_panel;
@property (strong) IBOutlet NSButton* sig_gen_startstop_button_400Hz;
@property (strong) IBOutlet NSButton* sig_gen_startstop_button_1kHz;
@property (strong) IBOutlet NSButton* sig_gen_startstop_button_custom;
@property (strong) IBOutlet NSButton* sig_gen_startstop_button_noise;
@end


@implementation SignalGeneratorViewController {
    SignalGenerator* sig_gen_400Hz;
    SignalGenerator* sig_gen_1kHz;
    SignalGenerator* sig_gen_custom;
    SignalGenerator* sig_gen_noise;

    bool sig_gen_400Hz_active;
    bool sig_gen_1kHz_active;
    bool sig_gen_custom_active;
    bool sig_gen_noise_active;
    
    uint32_t custom_type_tag;
    float custom_frequency;
    
    NSString* output_device;
}

-(void)showPanel {
    NSPanel* panel = (NSPanel*)[[self view] window];
    [_sig_gen_panel setFloatingPanel:YES];
    [_sig_gen_panel orderFront:nil];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    NSLog(@"Sig gen viewDidLoad");
    // Do view setup here.
    sig_gen_400Hz = [[SignalGenerator alloc] initWithSampleRate:48000 numberOfChannels:2];
    [sig_gen_400Hz configureWithType:SINE frequency:400];
    sig_gen_1kHz = [[SignalGenerator alloc] initWithSampleRate:48000 numberOfChannels:2];
    [sig_gen_1kHz configureWithType:SINE frequency:1000];
    sig_gen_custom = [[SignalGenerator alloc] initWithSampleRate:48000 numberOfChannels:2];
    [sig_gen_custom configureWithType:SINE frequency:0];
    custom_type_tag = 4;
    custom_frequency = 0.0f;
    sig_gen_noise = [[SignalGenerator alloc] initWithSampleRate:48000 numberOfChannels:2];
    [sig_gen_noise configureWithType:NOISE_WHITE frequency:0];
    
    /* HOKEY: Need to add this in here because the output device may have been changed before this view loaded. */
    [sig_gen_400Hz setOutputDevice:output_device active:sig_gen_400Hz_active];
    [sig_gen_1kHz setOutputDevice:output_device active:sig_gen_1kHz_active];
    [sig_gen_custom setOutputDevice:output_device active:sig_gen_custom_active];
    [sig_gen_noise setOutputDevice:output_device active:sig_gen_noise_active];
}

-(void)viewWillDisappear {
    
}

-(IBAction) setFunctionType400Hz:(id)sender {
    [sig_gen_400Hz configureWithType:(SIG_GEN_TYPE)[sender tag]
                      frequency:400];
}

-(IBAction) setFunctionType1kHz:(id)sender {
    [sig_gen_1kHz configureWithType:(SIG_GEN_TYPE)[sender tag]
                      frequency:1000];
}

-(IBAction) setFunctionTypeCustom:(id)sender {
    custom_type_tag = (uint32_t)[sender tag];
    [sig_gen_custom configureWithType:(SIG_GEN_TYPE)[sender tag]
                             frequency:custom_frequency];
}

-(IBAction) setFunctionFrequencyCustom:(id)sender {
    custom_frequency = [[sender cell] floatValue];
    [sig_gen_custom configureWithType:custom_type_tag
                            frequency: custom_frequency];
}

-(IBAction) setFunctionTypeNoise:(id)sender {
    [sig_gen_noise configureWithType:(SIG_GEN_TYPE)[sender tag]
                      frequency:0];
}


-(IBAction) setGain400Hz:(id)sender {
    [sig_gen_400Hz adjustVolume:[sender floatValue]];
}

-(IBAction) setGain1kHz:(id)sender {
    [sig_gen_1kHz adjustVolume:[sender floatValue]];
}

-(IBAction) setGainCustom:(id)sender {
    [sig_gen_custom adjustVolume:[sender floatValue]];
}

-(IBAction) setGainNoise:(id)sender {
    [sig_gen_noise adjustVolume:[sender floatValue]];
}

-(IBAction) startStop400Hz:(id)sender {
    if(sig_gen_400Hz_active) {
        [sig_gen_400Hz stopSignal];
        [_sig_gen_startstop_button_400Hz setTitle:@"Start"];
    }
    else {
        [sig_gen_400Hz startSignal];
        [_sig_gen_startstop_button_400Hz setTitle:@"Stop"];
    }
    
    sig_gen_400Hz_active = !sig_gen_400Hz_active;
}

-(IBAction) startStop1kHz:(id)sender {
    if(sig_gen_1kHz_active) {
        [sig_gen_1kHz stopSignal];
        [_sig_gen_startstop_button_1kHz setTitle:@"Start"];
    }
    else {
        [sig_gen_1kHz startSignal];
        [_sig_gen_startstop_button_1kHz setTitle:@"Stop"];
    }
    
    sig_gen_1kHz_active = !sig_gen_1kHz_active;
}

-(IBAction) startStopCustom:(id)sender {
    if(sig_gen_custom_active) {
        [sig_gen_custom stopSignal];
        [_sig_gen_startstop_button_custom setTitle:@"Start"];
    }
    else {
        [sig_gen_custom startSignal];
        [_sig_gen_startstop_button_custom setTitle:@"Stop"];
    }
    
    sig_gen_custom_active = !sig_gen_custom_active;
}

-(IBAction) startStopNoise:(id)sender {
    if(sig_gen_noise_active) {
        [sig_gen_noise stopSignal];
        [_sig_gen_startstop_button_noise setTitle:@"Start"];
    }
    else {
        [sig_gen_noise startSignal];
        [_sig_gen_startstop_button_noise setTitle:@"Stop"];
    }
    
    sig_gen_noise_active = !sig_gen_noise_active;
}

-(void) outputDeviceChanged:(NSString*)output_device_uid {
    output_device = [NSString stringWithString:output_device_uid];
    [sig_gen_400Hz setOutputDevice:output_device active:sig_gen_400Hz_active];
    [sig_gen_1kHz setOutputDevice:output_device active:sig_gen_1kHz_active];
    [sig_gen_custom setOutputDevice:output_device active:sig_gen_custom_active];
    [sig_gen_noise setOutputDevice:output_device active:sig_gen_noise_active];
}

@end
