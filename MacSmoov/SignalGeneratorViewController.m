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
@property (strong) IBOutlet NSButton* sig_gen_startstop_button_1;
@property (strong) IBOutlet NSButton* sig_gen_startstop_button_2;
@property (strong) IBOutlet NSButton* sig_gen_startstop_button_3;
@property (strong) IBOutlet NSButton* sig_gen_startstop_button_4;
@end


@implementation SignalGeneratorViewController {
    SignalGenerator* sig_gen1;
    SignalGenerator* sig_gen2;
    SignalGenerator* sig_gen3;
    SignalGenerator* sig_gen4;

    bool sig_gen1_active;
    bool sig_gen2_active;
    bool sig_gen3_active;
    bool sig_gen4_active;
}

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
    sig_gen1 = [[SignalGenerator alloc] initWithSampleRate:48000 numberOfChannels:2];
    sig_gen2 = [[SignalGenerator alloc] initWithSampleRate:48000 numberOfChannels:2];
    sig_gen3 = [[SignalGenerator alloc] initWithSampleRate:48000 numberOfChannels:2];
    sig_gen4 = [[SignalGenerator alloc] initWithSampleRate:48000 numberOfChannels:2];
}

-(void)viewWillDisappear {
    
}

-(SIG_GEN_TYPE) convert_type_button_to_type:(NSString*) identifier {
    if([identifier isEqualTo:@"sine_400"] || [identifier isEqualTo:@"sine_1000"]) {
        return SINE;
    }
    else if([identifier isEqualTo:@"noise_white"]) {
        return NOISE_WHITE;
    }
    else if([identifier isEqualTo:@"noise_pink"]) {
        return NOISE_PINK;
    }
    
    return UNKNOWN;
}

-(uint32_t) convert_type_button_to_frequency:(NSString*) identifier {
    if([identifier isEqualTo:@"sine_400"]) {
        return 400;
    }
    else if([identifier isEqualTo:@"sine_1000"]) {
        return 1000;
    }
    
    return 0;
}

-(IBAction) functionTypeGen1:(id)sender {
    [sig_gen1 configureWithType:[self convert_type_button_to_type:[sender identifier]]
                      frequency:[self convert_type_button_to_frequency:[sender identifier]]];
}

-(IBAction) functionTypeGen2:(id)sender {
    [sig_gen2 configureWithType:[self convert_type_button_to_type:[sender identifier]]
                      frequency:[self convert_type_button_to_frequency:[sender identifier]]];
}

-(IBAction) functionTypeGen3:(id)sender {
    [sig_gen3 configureWithType:[self convert_type_button_to_type:[sender identifier]]
                      frequency:[self convert_type_button_to_frequency:[sender identifier]]];
}

-(IBAction) functionTypeGen4:(id)sender {
    [sig_gen4 configureWithType:[self convert_type_button_to_type:[sender identifier]]
                      frequency:[self convert_type_button_to_frequency:[sender identifier]]];
}

-(IBAction) gainAdjustGen1:(id)sender {
    [sig_gen1 adjustVolume:[sender floatValue]];
    
}

-(IBAction) gainAdjustGen2:(id)sender {
    [sig_gen2 adjustVolume:[sender floatValue]];
}

-(IBAction) gainAdjustGen3:(id)sender {
    [sig_gen3 adjustVolume:[sender floatValue]];
}

-(IBAction) gainAdjustGen4:(id)sender {
    [sig_gen4 adjustVolume:[sender floatValue]];
}

-(IBAction) startStopGen1:(id)sender {
    if(sig_gen1_active) {
        [sig_gen1 stopSignal];
        [_sig_gen_startstop_button_1 setTitle:@"Start"];
    }
    else {
        [sig_gen1 startSignal];
        [_sig_gen_startstop_button_1 setTitle:@"Stop"];
    }
    
    sig_gen1_active = !sig_gen1_active;
}

-(IBAction) startStopGen2:(id)sender {
    if(sig_gen2_active) {
        [sig_gen2 stopSignal];
        [_sig_gen_startstop_button_2 setTitle:@"Start"];
    }
    else {
        [sig_gen2 startSignal];
        [_sig_gen_startstop_button_2 setTitle:@"Stop"];
    }
    
    sig_gen2_active = !sig_gen2_active;
}

-(IBAction) startStopGen3:(id)sender {
    if(sig_gen3_active) {
        [sig_gen3 stopSignal];
        [_sig_gen_startstop_button_3 setTitle:@"Start"];
    }
    else {
        [sig_gen3 startSignal];
        [_sig_gen_startstop_button_3 setTitle:@"Stop"];
    }
    
    sig_gen3_active = !sig_gen3_active;
}

-(IBAction) startStopGen4:(id)sender {
    if(sig_gen4_active) {
        [sig_gen4 stopSignal];
        [_sig_gen_startstop_button_4 setTitle:@"Start"];
    }
    else {
        [sig_gen4 startSignal];
        [_sig_gen_startstop_button_4 setTitle:@"Stop"];
    }
    
    sig_gen4_active = !sig_gen4_active;
}



@end
