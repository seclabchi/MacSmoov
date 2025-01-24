//
//  AGCControlsView.m
//  MacSmoov
//
//  Created by Paul Zaremba on 8/28/23.
//

#import "AGCControlsView.h"
#import "core/core_common.h"

@interface AGCControlsView () {
    id<AGCControlsDelegate> delegate;
}

@end


@implementation AGCControlsView

/*
 * TODO: Store min/max values for everything as well and bind to the sliders?
 */

-(id) initWithSettings:(AGC_PARAMS) settings delegate:(id)agc_delegate {
    self = [super init];
    if(self) {
        delegate = agc_delegate;
        _agc_settings = settings;
        _enabled  = false;
        _mute_lo = _agc_settings.mute_lo;
        _mute_hi = _agc_settings.mute_hi;
        _drive = _agc_settings.drive;
        _target = _agc_settings.target;
        _release_master = _agc_settings.release_master;
        _release_bass = _agc_settings.release_bass;
        _gate_thresh = _agc_settings.gate_thresh;
        _bass_coupling = _agc_settings.bass_coupling;
        _window_size = _agc_settings.window_size;
        _window_release = _agc_settings.window_release;
        _ratio = _agc_settings.ratio;
        _bass_thresh = _agc_settings.bass_thresh;
        _idle_gain = _agc_settings.idle_gain;
        _attack_master = _agc_settings.attack_master;
        _attack_bass = _agc_settings.attack_bass;
        _post_gain = _agc_settings.post_gain;
    }
    
    return self;
}


- (void)viewDidLoad {
    [super viewDidLoad];
    NSLog(@"AGCControlsView viewDidLoad");
    
}

-(void)showPanel {
    NSPanel* panel = (NSPanel*)[[self view] window];
    [panel setFloatingPanel:YES];
    [panel orderFront:nil];
}

-(IBAction) factory_reset:(NSButton*)sender {
    NSLog(@"Factory reset pressed.");
}

-(IBAction) setting_changed:(NSSlider*) sender {
    NSLog(@"Setting changed: %@", sender);
    _agc_settings.drive = _drive;
    _agc_settings.release_master = _release_master;
    _agc_settings.release_bass = _release_bass;
    _agc_settings.gate_thresh = _gate_thresh;
    _agc_settings.bass_coupling = _bass_coupling;
    _agc_settings.window_size = _window_size;
    _agc_settings.window_release = _window_release;
    _agc_settings.ratio = _ratio;
    _agc_settings.bass_thresh = _bass_thresh;
    _agc_settings.idle_gain = _idle_gain;
    _agc_settings.attack_master = _attack_master;
    _agc_settings.attack_bass = _attack_bass;
    _agc_settings.post_gain = _post_gain;
    _agc_settings.mute_lo = _mute_lo;
    _agc_settings.mute_hi = _mute_hi;
    [delegate agc_params_changed:_agc_settings];
}


- (void)controlTextDidChange:(NSNotification *)obj {
    NSLog(@"Control text changed.");
}

@end
