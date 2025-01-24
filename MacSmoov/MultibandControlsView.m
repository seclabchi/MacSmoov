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
    .target = -18.0,
    .release = 0.100,
    .thresh = 0.0,
    .ratio = 1.5,
    .attack = .050
};

COMPRESSOR_PARAMS factory_comp2 = {
    .target = -18.0,
    .release = 0.100,
    .thresh = 0.0,
    .ratio = 1.5,
    .attack = .050
};

COMPRESSOR_PARAMS factory_comp3 = {
    .target = -18.0,
    .release = 0.100,
    .thresh = 0.0,
    .ratio = 1.5,
    .attack = .050
};

COMPRESSOR_PARAMS factory_comp4 = {
    .target = -18.0,
    .release = 0.100,
    .thresh = 0.0,
    .ratio = 1.5,
    .attack = .050
};

COMPRESSOR_PARAMS factory_comp5 = {
    .target = -18.0,
    .release = 0.100,
    .thresh = 0.0,
    .ratio = 1.5,
    .attack = .050
};

/*===================================*/
/* LIMITERS                          */
/*===================================*/
COMPRESSOR_PARAMS factory_lim1 = {
    .target = -18.0,
    .release = 0.050,
    .ratio = 20.0,
    .attack = 0.050,
};

COMPRESSOR_PARAMS factory_lim2 = {
    .target = -18.0,
    .release = 0.050,
    .ratio = 20.0,
    .attack = 0.050,
};

COMPRESSOR_PARAMS factory_lim3 = {
    .target = -18.0,
    .release = 0.050,
    .ratio = 20.0,
    .attack = 0.050,
};

COMPRESSOR_PARAMS factory_lim4 = {
    .target = -18.0,
    .release = 0.050,
    .ratio = 20.0,
    .attack = 0.050,
};

COMPRESSOR_PARAMS factory_lim5 = {
    .target = -18.0,
    .release = 0.050,
    .ratio = 20.0,
    .attack = 0.050,
};


-(id) initWithSettings:(MULTIBAND_PARAMS) _settings delegate:(id)mb_delegate {
    self = [super init];
    if(self) {
        delegate = mb_delegate;
        _mb_settings = _settings;
        _mb_obj_ctrl = [[MultibandObjectController alloc] init];
        [_mb_obj_ctrl setAllSettings: _settings];
        _objctrl.content = _mb_obj_ctrl;
    }
    
    return self;
}


- (void)viewDidLoad {
    [super viewDidLoad];
    // Do view setup here.
    NSLog(@"MultibandControlsView viewDidLoad");
}

-(IBAction) band_select:(NSButton*)sender {
    NSInteger band_num = [[sender title] integerValue];
    NSLog(@"Band %d selected.", (uint8_t)band_num);
    [_mb_obj_ctrl selectBand:band_num];
}

-(void)showPanel {
    NSPanel* panel = (NSPanel*)[[self view] window];
    [panel setFloatingPanel:YES];
    [panel orderFront:nil];
}

-(IBAction) setting_changed:(id) sender {
    NSLog(@"Setting changed: %@", sender);
    [_mb_obj_ctrl dumpSettings];
    _mb_settings = [_mb_obj_ctrl getAllSettings];
    [delegate multiband_params_changed:_mb_settings];

}

@end
