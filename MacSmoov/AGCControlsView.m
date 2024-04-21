//
//  AGCControlsView.m
//  MacSmoov
//
//  Created by Paul Zaremba on 8/28/23.
//

#import "AGCControlsView.h"
#import "core/core_common.h"

@interface AGCControlsView () {
    NSUserDefaults* prefs;
    AGC_PARAMS agc_params;
    id<AGCControlsDelegate> delegate;
}

-(void) initFloatPref:(NSString*)prefstr value:(float)val factory_reset:(BOOL)force_factory;
-(void) setBoolPref:(NSString*)prefstr value:(BOOL)val factory_reset:(BOOL)force_factory;
-(void) read_prefs;

//-(void) populate_ui_elements:(uint32_t)current_band;

@end


@implementation AGCControlsView


AGC_PARAMS factory_agc_params = {
    .enabled = true,
    .drive = -40.0,
    .release_master = 0.542,
    .release_bass = 0.542,
    .gate_thresh = -35.0,
    .bass_coupling = 0.3,
    .window_size = -3.0,
    .window_release = 60,
    .ratio = 200.0,
    .bass_thresh = 0.0,
    .idle_gain = 0.0,
    .attack_master = 8.685,
    .attack_bass = 8.685,
    .post_gain = 17.0
};


-(id) initWithPrefs:(NSUserDefaults*) defaults delegate:(id) agc_delegate {
    self = [super init];
    if(self) {
        prefs = defaults;
        delegate = agc_delegate;
        
        [self setupPrefsIfNeeded:NO];
        [self read_prefs];
        //[delegate multiband_params_changed:mb_params];
    }
    
    return self;
}


- (void)viewDidLoad {
    [super viewDidLoad];
    // Do view setup here.
    NSLog(@"AGCControlsView viewDidLoad");
    
}

-(void) populate_ui_elements:(uint32_t)current_band {
    
}

-(void) setPrefs:(NSUserDefaults*) defaults {
    prefs = defaults;
}

-(void)showPanel {
    NSPanel* panel = (NSPanel*)[[self view] window];
    [panel setFloatingPanel:YES];
    [panel orderFront:nil];
}

-(IBAction) setting_changed:(id) sender {
    NSLog(@"Setting changed: %@", sender);
}

-(void) read_prefs {
    [delegate agc_params_changed:agc_params];
}

-(void) setFloatPref:(NSString*)prefstr value:(float)val {
    [prefs setObject:[NSString stringWithFormat:@"%f", val] forKey:prefstr];
}

-(void) setBoolPref:(NSString*)prefstr value:(BOOL)val {
    [prefs setBool:val forKey:prefstr];
}

/* At startup, check for the presence of these prefs.  If they don't exist, create them. */
-(void) setupPrefsIfNeeded:(BOOL)factory_reset {
    
}

-(IBAction) factory_reset:(NSButton*)sender {
    [self setupPrefsIfNeeded:YES];
    [self read_prefs];
}

- (void)controlTextDidChange:(NSNotification *)obj {
    NSLog(@"Control text changed.");
}

@end
