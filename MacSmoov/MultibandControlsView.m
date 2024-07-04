//
//  MultibandControlsView.m
//  MacSmoov
//
//  Created by Paul Zaremba on 8/28/23.
//

#import "MultibandControlsView.h"
#import "core/core_common.h"

@interface MultibandControlsView () {
    id<MultibandControlsDelegate> delegate;
}

-(void) set_band_pref:(COMPRESSOR_PARAMS) params band_num:(uint32_t) num is_comp:(bool)comp;
//-(void) initFloatPref:(NSString*)prefstr value:(float)val factory_reset:(BOOL)force_factory;
//-(void) setBoolPref:(NSString*)prefstr value:(BOOL)val factory_reset:(BOOL)force_factory;
-(void) read_prefs;

-(void) populate_ui_elements:(uint32_t)current_band;

@end


@implementation MultibandControlsView

@synthesize b1_drive;

COMPRESSOR_PARAMS factory_comp1 = {
    .drive = -42.0,
    .release = 1.600,
    .gate_thresh = -57.0,
    .use_coupling = false,
    .coupling = 0.0,
    .window_size = 99.0,
    .window_release = 99.0,
    .ratio = 1.2,
    .idle_gain = 0.0,
    .attack = 0.800,
    .post_gain = 0.0
};

COMPRESSOR_PARAMS factory_comp2 = {
    .drive = -42.0,
    .release = 0.600,
    .gate_thresh = -57.0,
    .use_coupling = false,
    .coupling = 0.0,
    .window_size = 99.0,
    .window_release = 99.0,
    .ratio = 1.3,
    .idle_gain = 0.0,
    .attack = 0.075,
    .post_gain = 0.0
};

COMPRESSOR_PARAMS factory_comp3 = {
    .drive = -42.0,
    .release = 0.450,
    .gate_thresh = -57.0,
    .use_coupling = false,
    .coupling = 0.0,
    .window_size = 99.0,
    .window_release = 99.0,
    .ratio = 1.3,
    .idle_gain = 0.0,
    .attack = 0.05625,
    .post_gain = 0.0
};

COMPRESSOR_PARAMS factory_comp4 = {
    .drive = -42.0,
    .release = 0.3375,
    .gate_thresh = -57.0,
    .use_coupling = true,
    .coupling = 0.20,
    .window_size = 99.0,
    .window_release = 99.0,
    .ratio = 1.3,
    .idle_gain = 0.0,
    .attack = 0.0421875,
    .post_gain = 0.0
};

COMPRESSOR_PARAMS factory_comp5 = {
    .drive = -42.0,
    .release = 0.253125,
    .gate_thresh = -57.0,
    .use_coupling = true,
    .coupling = 0.20,
    .window_size = 99.0,
    .window_release = 99.0,
    .ratio = 1.3,
    .idle_gain = 0.0,
    .attack = 0.031640625,
    .post_gain = 0.0
};

/*===================================*/
/* LIMITERS                          */
/*===================================*/
COMPRESSOR_PARAMS factory_lim1 = {
    .drive = -35.0,
    .release = 0.075,
    .gate_thresh = -100.0,
    .use_coupling = false,
    .coupling = 0.0,
    .window_size = 0.0,
    .window_release = 0.0,
    .ratio = 100,
    .idle_gain = 0.0,
    .attack = 0.050,
    .post_gain = 5.0
};

COMPRESSOR_PARAMS factory_lim2 = {
    .drive = -38.0,
    .release = 0.075,
    .gate_thresh = -100.0,
    .use_coupling = false,
    .coupling = 0.0,
    .window_size = 0.0,
    .window_release = 0.0,
    .ratio = 100,
    .idle_gain = 0.0,
    .attack = 0.075,
    .post_gain = 4.0
};

COMPRESSOR_PARAMS factory_lim3 = {
    .drive = -41.0,
    .release = 0.05625,
    .gate_thresh = -100.0,
    .use_coupling = false,
    .coupling = 0.0,
    .window_size = 0.0,
    .window_release = 0.0,
    .ratio = 100,
    .idle_gain = 0.0,
    .attack = 0.05625,
    .post_gain = 4.0
};

COMPRESSOR_PARAMS factory_lim4 = {
    .drive = -44.0,
    .release = 0.0421875,
    .gate_thresh = -100.0,
    .use_coupling = false,
    .coupling = 0.0,
    .window_size = 0.0,
    .window_release = 0.0,
    .ratio = 100,
    .idle_gain = 0.0,
    .attack = 0.0421875,
    .post_gain = 5.0
};

COMPRESSOR_PARAMS factory_lim5 = {
    .drive = -47.0,
    .release = 0.031640625,
    .gate_thresh = -100.0,
    .use_coupling = false,
    .coupling = 0.0,
    .window_size = 0.0,
    .window_release = 0.0,
    .ratio = 100,
    .idle_gain = 0.0,
    .attack = 0.031640625,
    .post_gain = 6.0
};


-(id) initWithSettings:(MULTIBAND_PARAMS) _settings delegate:(id)mb_delegate {
    self = [super init];
    if(self) {
        delegate = mb_delegate;
        _mb_settings = _settings;
    }
    
    return self;
}


- (void)viewDidLoad {
    [super viewDidLoad];
    // Do view setup here.
    NSLog(@"MultibandControlsView viewDidLoad");
}

-(IBAction) band_select:(NSButton*)sender {
    NSLog(@"Band %@ selected.", [sender title]);
}

-(void)showPanel {
    NSPanel* panel = (NSPanel*)[[self view] window];
    [panel setFloatingPanel:YES];
    [panel orderFront:nil];
}

-(IBAction) setting_changed:(id) sender {
    NSLog(@"Setting changed: %@", sender);
    [delegate multiband_params_changed:_mb_settings];

}

@end
