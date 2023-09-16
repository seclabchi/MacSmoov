//
//  MultibandControlsView.m
//  MacSmoov
//
//  Created by Paul Zaremba on 8/28/23.
//

#import "MultibandControlsView.h"
#import "core/core_common.h"

@interface MultibandControlsView () {
    NSUserDefaults* prefs;
    uint8_t current_band;
    MULTIBAND_PARAMS mb_params;
    id<MultibandControlsDelegate> delegate;
}

-(void) set_band_pref:(COMPRESSOR_PARAMS) params band_num:(uint32_t) num is_comp:(bool)comp;
-(void) initFloatPref:(NSString*)prefstr value:(float)val factory_reset:(BOOL)force_factory;
-(void) setBoolPref:(NSString*)prefstr value:(BOOL)val factory_reset:(BOOL)force_factory;
-(void) read_prefs;

-(void) populate_ui_elements:(uint32_t)current_band;

@end



@implementation MultibandControlsView

COMPRESSOR_PARAMS factory_comp1 = {
    .drive = -42.0,
    .release = 1.200,
    .gate_thresh = -57.0,
    .use_coupling = false,
    .coupling = 0.0,
    .window_size = 99.0,
    .window_release = 99.0,
    .ratio = 1.3,
    .idle_gain = 0.0,
    .attack = 0.100,
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
    .drive = -34.0,
    .release = 0.100,
    .gate_thresh = -100.0,
    .use_coupling = false,
    .coupling = 0.0,
    .window_size = 0.0,
    .window_release = 0.0,
    .ratio = 100,
    .idle_gain = 0.0,
    .attack = 0.100,
    .post_gain = 3.0
};

COMPRESSOR_PARAMS factory_lim2 = {
    .drive = -37.0,
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
    .drive = -40.0,
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
    .drive = -43.0,
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
    .drive = -46.0,
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



-(id) initWithPrefs:(NSUserDefaults*) defaults delegate:(id) mb_delegate {
    self = [super init];
    if(self) {
        prefs = defaults;
        delegate = mb_delegate;
        
        [self setupPrefsIfNeeded:NO];
        [self read_prefs];
        [delegate multiband_params_changed:mb_params];
    }
    
    return self;
}


- (void)viewDidLoad {
    [super viewDidLoad];
    // Do view setup here.
    NSLog(@"MultibandControlsView viewDidLoad");
    
    //default band is 1 on startup
    current_band = 1;
    
    [self populate_ui_elements:current_band];
    
    //Read band enable settings
    NSControlStateValue bands_enabled[5];
    
    bands_enabled[0] = [prefs boolForKey:@"ENABLE_B1"];
    bands_enabled[1] = [prefs boolForKey:@"ENABLE_B2"];
    bands_enabled[2] = [prefs boolForKey:@"ENABLE_B3"];
    bands_enabled[3] = [prefs boolForKey:@"ENABLE_B4"];
    bands_enabled[4] = [prefs boolForKey:@"ENABLE_B5"];
    
    [_enable_b1 setState: bands_enabled[0]];
    [_enable_b2 setState: bands_enabled[1]];
    [_enable_b3 setState: bands_enabled[2]];
    [_enable_b4 setState: bands_enabled[3]];
    [_enable_b5 setState: bands_enabled[4]];
}

-(void) populate_ui_elements:(uint32_t)current_band {
    NSString* bandstr = [NSString stringWithFormat:@"B%d", current_band];
    NSString* propstr;
    
    propstr = [NSString stringWithFormat:@"MB_COMP_DRIVE_%@", bandstr];
    [_compressor_drive setFloatValue: [prefs floatForKey:propstr]];
    propstr = [NSString stringWithFormat:@"MB_COMP_RELEASE_%@", bandstr];
    [_compressor_release setFloatValue: [prefs floatForKey:propstr] * 1000];
    propstr = [NSString stringWithFormat:@"MB_COMP_GATE_THRESH_%@", bandstr];
    [_compressor_gate_thresh setFloatValue: [prefs floatForKey:propstr]];
    propstr = [NSString stringWithFormat:@"MB_COMP_RATIO_%@", bandstr];
    [_compressor_ratio setFloatValue: [prefs floatForKey:propstr]];
    propstr = [NSString stringWithFormat:@"MB_COMP_IDLE_GAIN_%@", bandstr];
    [_compressor_idle_gain setFloatValue: [prefs floatForKey:propstr]];
    propstr = [NSString stringWithFormat:@"MB_COMP_ATTACK_%@", bandstr];
    [_compressor_attack setFloatValue: [prefs floatForKey:propstr] * 1000];
    
    [_stepper_compressor_drive setFloatValue: [_compressor_drive floatValue]];
    [_stepper_compressor_release setFloatValue: [_compressor_release floatValue] * 1000];
    [_stepper_compressor_gate_thresh setFloatValue: [_compressor_gate_thresh floatValue]];
    [_stepper_compressor_ratio setFloatValue: [_compressor_ratio floatValue]];
    [_stepper_compressor_idle_gain setFloatValue: [_compressor_idle_gain floatValue]];
    [_stepper_compressor_attack setFloatValue: [_compressor_attack floatValue] * 1000];
    
    [_stepper_limiter_drive setFloatValue: [_limiter_drive floatValue]];
    [_stepper_limiter_release setFloatValue: [_limiter_release floatValue] * 1000];
    [_stepper_limiter_ratio setFloatValue: [_limiter_ratio floatValue]];
    [_stepper_limiter_idle_gain setFloatValue: [_limiter_idle_gain floatValue]];
    [_stepper_limiter_attack setFloatValue: [_limiter_attack floatValue] * 1000];
    [_stepper_limiter_output_gain setFloatValue: [_limiter_output_gain floatValue]];
}

-(IBAction) band_select:(NSButton*)sender {
    NSLog(@"Band %lu selected.", (long)[sender tag]);
    current_band = (long)[sender tag];
    [self populate_ui_elements:current_band];
}

-(void) setPrefs:(NSUserDefaults*) defaults {
    prefs = defaults;
}

-(void)showPanel {
    NSPanel* panel = (NSPanel*)[[self view] window];
    [panel setFloatingPanel:YES];
    [panel orderFront:nil];
}

-(IBAction) value_changed_compressor:(NSTextField*)sender {
    NSString* bandstr = [NSString stringWithFormat:@"B%d", current_band];
    NSString* propstr;
    uint32_t band_index = current_band - 1;
    
    switch([sender tag]) {
        case 0: //compressor_drive
            mb_params.comp_params[band_index].drive = [sender floatValue];
            break;
        case 1: //compressor_release
            mb_params.comp_params[band_index].release = [sender floatValue] / 1000.0f;
            break;
        case 2: //compressor_gate_thresh
            mb_params.comp_params[band_index].gate_thresh = [sender floatValue];
            break;
        case 3: //compressor_ratio
            mb_params.comp_params[band_index].ratio = [sender floatValue];
            break;
        case 4: //compressor_idle_gain
            mb_params.comp_params[band_index].idle_gain = [sender floatValue];
            break;
        case 5: //compressor_attack
            mb_params.comp_params[band_index].attack = [sender floatValue] / 1000.0f;
            break;
            
    }
    
    [self set_band_pref:mb_params.comp_params[band_index] band_num:current_band is_comp:YES];
    [delegate multiband_params_changed:mb_params];
}

-(IBAction) stepper_changed_compressor:(NSStepper*)sender {
    uint32_t band_index = current_band - 1;
    
    switch([sender tag]) {
        case 0: //compressor_drive
            [_compressor_drive setFloatValue:[_stepper_compressor_drive floatValue]];
            break;
        case 1: //compressor_release
            [_compressor_release setFloatValue:[_stepper_compressor_release floatValue]];
            break;
        case 2: //compressor_gate_thresh
            [_compressor_gate_thresh setFloatValue:[_stepper_compressor_gate_thresh floatValue]];
            break;
        case 3: //compressor_ratio
            [_compressor_ratio setFloatValue:[_stepper_compressor_ratio floatValue]];
            break;
        case 4: //compressor_idle_gain
            [_compressor_idle_gain setFloatValue:[_stepper_compressor_idle_gain floatValue]];
            break;
        case 5: //compressor_attack
            [_compressor_attack setFloatValue:[_stepper_compressor_attack floatValue]];
            break;
            
    }
    
    [self set_band_pref:mb_params.comp_params[band_index] band_num:current_band is_comp:YES];
    [delegate multiband_params_changed:mb_params];
}

-(IBAction) value_changed_limiter:(NSTextField*)sender {
    NSString* bandstr = [NSString stringWithFormat:@"B%d", current_band];
    NSString* propstr;
    uint32_t band_index = current_band - 1;
    
    switch([sender tag]) {
        case 0: //limiter_drive
            break;
        case 1: //limiter_release
            break;
        case 2: //limiter_ratio
            break;
        case 3: //limiter_idle_gain
            break;
        case 4: //limiter_attack
            break;
        case 5: //limiter_output_gain
            break;
    }
    
    [self set_band_pref:mb_params.lim_params[band_index] band_num:current_band is_comp:NO];
    [delegate multiband_params_changed:mb_params];
}

-(IBAction) stepper_changed_limiter:(NSStepper*)sender {
    switch([sender tag]) {
        case 0: //limiter_drive
            [_limiter_drive setFloatValue:[_stepper_limiter_drive floatValue]];
            break;
        case 1: //limiter_release
            [_limiter_release setFloatValue:[_stepper_limiter_drive floatValue]];
            break;
        case 2: //limiter_ratio
            [_limiter_ratio setFloatValue:[_stepper_limiter_drive floatValue]];
            break;
        case 3: //limiter_idle_gain
            [_limiter_idle_gain setFloatValue:[_stepper_limiter_drive floatValue]];
            break;
        case 4: //limiter_attack
            [_limiter_attack setFloatValue:[_stepper_limiter_drive floatValue]];
            break;
        case 5: //limiter_output_gain
            [_limiter_output_gain setFloatValue:[_stepper_limiter_drive floatValue]];
            break;
    }
    
    [delegate multiband_params_changed:mb_params];
}

-(IBAction) bandEnableChanged:(NSButton*)sender {
    NSControlStateValue states[5];

    NSString* bandstr;
    NSString* keystr;
    
    states[0] = _enable_b1.state;
    states[1] = _enable_b2.state;
    states[2] = _enable_b3.state;
    states[3] = _enable_b4.state;
    states[4] = _enable_b5.state;
    
    for(uint32_t i = 0; i < 5; i++) {
        bandstr = [NSString stringWithFormat:@"B%d", i+1];
        keystr = [NSString stringWithFormat:@"ENABLE_%@", bandstr];
        [self setBoolPref:keystr value:states[i]];
    }
    
    [delegate band_enablement_changed:states];
}

-(void) read_prefs {
    NSString* bandstr;
    NSString* keystr;
    
    NSControlStateValue bands_enabled[5];
    
    
    for(uint32_t i = 0; i < 5; i++) {
        bandstr = [NSString stringWithFormat:@"B%d", i+1];
        keystr = [NSString stringWithFormat:@"ENABLE_%@", bandstr];
        bands_enabled[i] = [prefs boolForKey:keystr];
    }
    
    [delegate band_enablement_changed:bands_enabled];
    
    for(uint32_t i = 0; i < 5; i++) {
        bandstr = [NSString stringWithFormat:@"B%d", i+1];
        keystr = [NSString stringWithFormat:@"MB_COMP_DRIVE_%@", bandstr];
        mb_params.comp_params[i].drive = [prefs floatForKey:keystr];
        keystr = [NSString stringWithFormat:@"MB_COMP_RELEASE_%@", bandstr];
        mb_params.comp_params[i].release = [prefs floatForKey:keystr];
        keystr = [NSString stringWithFormat:@"MB_COMP_GATE_THRESH_%@", bandstr];
        mb_params.comp_params[i].gate_thresh = [prefs floatForKey:keystr];
        keystr = [NSString stringWithFormat:@"MB_COMP_USE_COUPLING_%@", bandstr];
        mb_params.comp_params[i].use_coupling = [prefs boolForKey:keystr];
        keystr = [NSString stringWithFormat:@"MB_COMP_COUPLING_%@", bandstr];
        mb_params.comp_params[i].coupling = [prefs floatForKey:keystr];
        keystr = [NSString stringWithFormat:@"MB_COMP_WINDOW_SIZE_%@", bandstr];
        mb_params.comp_params[i].window_size = [prefs floatForKey:keystr];
        keystr = [NSString stringWithFormat:@"MB_COMP_WINDOW_RELEASE_%@", bandstr];
        mb_params.comp_params[i].window_release = [prefs floatForKey:keystr];
        keystr = [NSString stringWithFormat:@"MB_COMP_RATIO_%@", bandstr];
        mb_params.comp_params[i].ratio = [prefs floatForKey:keystr];
        keystr = [NSString stringWithFormat:@"MB_COMP_IDLE_GAIN_%@", bandstr];
        mb_params.comp_params[i].idle_gain = [prefs floatForKey:keystr];
        keystr = [NSString stringWithFormat:@"MB_COMP_ATTACK_%@", bandstr];
        mb_params.comp_params[i].attack = [prefs floatForKey:keystr];
        keystr = [NSString stringWithFormat:@"MB_COMP_POST_GAIN_%@", bandstr];
        mb_params.comp_params[i].post_gain = [prefs floatForKey:keystr];
        /* LIMITERS */
        keystr = [NSString stringWithFormat:@"MB_LIM_DRIVE_%@", bandstr];
        mb_params.lim_params[i].drive = [prefs floatForKey:keystr];
        keystr = [NSString stringWithFormat:@"MB_LIM_RELEASE_%@", bandstr];
        mb_params.lim_params[i].release = [prefs floatForKey:keystr];
        keystr = [NSString stringWithFormat:@"MB_LIM_GATE_THRESH_%@", bandstr];
        mb_params.lim_params[i].gate_thresh = [prefs floatForKey:keystr];
        keystr = [NSString stringWithFormat:@"MB_LIM_USE_COUPLING_%@", bandstr];
        mb_params.lim_params[i].use_coupling = [prefs boolForKey:keystr];
        keystr = [NSString stringWithFormat:@"MB_LIM_WINDOW_SIZE_%@", bandstr];
        mb_params.lim_params[i].window_size = [prefs floatForKey:keystr];
        keystr = [NSString stringWithFormat:@"MB_LIM_WINDOW_RELEASE_%@", bandstr];
        mb_params.lim_params[i].window_release = [prefs floatForKey:keystr];
        keystr = [NSString stringWithFormat:@"MB_LIM_COUPLING_%@", bandstr];
        mb_params.lim_params[i].coupling = [prefs floatForKey:keystr];
        keystr = [NSString stringWithFormat:@"MB_LIM_RATIO_%@", bandstr];
        mb_params.lim_params[i].ratio = [prefs floatForKey:keystr];
        keystr = [NSString stringWithFormat:@"MB_LIM_IDLE_GAIN_%@", bandstr];
        mb_params.lim_params[i].idle_gain = [prefs floatForKey:keystr];
        keystr = [NSString stringWithFormat:@"MB_LIM_ATTACK_%@", bandstr];
        mb_params.lim_params[i].attack = [prefs floatForKey:keystr];
        keystr = [NSString stringWithFormat:@"MB_LIM_POST_GAIN_%@", bandstr];
        mb_params.lim_params[i].post_gain = [prefs floatForKey:keystr];
    }
    
    [delegate multiband_params_changed:mb_params];
    
}

-(void) set_band_pref:(COMPRESSOR_PARAMS) params band_num:(uint32_t) num is_comp:(bool)comp {
    NSString* bandstr;
    NSString* typestr;
    NSString* prefstr;
    NSString* paramstr;
    
    switch(num) {
        case 1:
            bandstr = @"B1";
            break;
        case 2:
            bandstr = @"B2";
            break;
        case 3:
            bandstr = @"B3";
            break;
        case 4:
            bandstr = @"B4";
            break;
        case 5:
            bandstr = @"B5";
            break;
    }
    
    if(comp) {
        typestr = @"COMP";
    }
    else {
        typestr = @"LIM";
    }
        
    paramstr = @"DRIVE";
    prefstr = [NSString stringWithFormat:@"MB_%@_%@_%@", typestr, paramstr, bandstr];
    [self setFloatPref:prefstr value:params.drive];
    paramstr = @"RELEASE";
    prefstr = [NSString stringWithFormat:@"MB_%@_%@_%@", typestr, paramstr, bandstr];
    [self setFloatPref:prefstr value:params.release];
    paramstr = @"GATE_THRESH";
    prefstr = [NSString stringWithFormat:@"MB_%@_%@_%@", typestr, paramstr, bandstr];
    [self setFloatPref:prefstr value:params.gate_thresh];
    paramstr = @"USE_COUPLING";
    prefstr = [NSString stringWithFormat:@"MB_%@_%@_%@", typestr, paramstr, bandstr];
    [self setBoolPref:prefstr value:params.use_coupling];
    paramstr = @"COUPLING";
    prefstr = [NSString stringWithFormat:@"MB_%@_%@_%@", typestr, paramstr, bandstr];
    [self setFloatPref:prefstr value:params.coupling];
    paramstr = @"WINDOW_SIZE";
    prefstr = [NSString stringWithFormat:@"MB_%@_%@_%@", typestr, paramstr, bandstr];
    [self setFloatPref:prefstr value:params.window_size];
    paramstr = @"WINDOW_RELEASE";
    prefstr = [NSString stringWithFormat:@"MB_%@_%@_%@", typestr, paramstr, bandstr];
    [self setFloatPref:prefstr value:params.window_release];
    paramstr = @"RATIO";
    prefstr = [NSString stringWithFormat:@"MB_%@_%@_%@", typestr, paramstr, bandstr];
    [self setFloatPref:prefstr value:params.ratio];
    paramstr = @"IDLE_GAIN";
    prefstr = [NSString stringWithFormat:@"MB_%@_%@_%@", typestr, paramstr, bandstr];
    [self setFloatPref:prefstr value:params.idle_gain];
    paramstr = @"ATTACK";
    prefstr = [NSString stringWithFormat:@"MB_%@_%@_%@", typestr, paramstr, bandstr];
    [self setFloatPref:prefstr value:params.attack];
    paramstr = @"POST_GAIN";
    prefstr = [NSString stringWithFormat:@"MB_%@_%@_%@", typestr, paramstr, bandstr];
    [self setFloatPref:prefstr value:params.post_gain];
    
}

-(void) setFloatPref:(NSString*)prefstr value:(float)val {
    [prefs setObject:[NSString stringWithFormat:@"%f", val] forKey:prefstr];
}

-(void) setBoolPref:(NSString*)prefstr value:(BOOL)val {
    [prefs setBool:val forKey:prefstr];
}

/* At startup, check for the presence of these prefs.  If they don't exist, create them. */
-(void) setupPrefsIfNeeded:(BOOL)factory_reset {
    NSString* bandstr;
    NSString* keystr;
    
    COMPRESSOR_PARAMS factory_comp[5];
    COMPRESSOR_PARAMS factory_lim[5];
    
    BOOL factory_band_enable = true;

    factory_comp[0] = factory_comp1;
    factory_comp[1] = factory_comp2;
    factory_comp[2] = factory_comp3;
    factory_comp[3] = factory_comp4;
    factory_comp[4] = factory_comp5;
    factory_lim[0] = factory_lim1;
    factory_lim[1] = factory_lim2;
    factory_lim[2] = factory_lim3;
    factory_lim[3] = factory_lim4;
    factory_lim[4] = factory_lim5;
    
    for(uint32_t i = 0; i < 5; i++) {
        bandstr = [NSString stringWithFormat:@"B%d", i+1];
        keystr = [NSString stringWithFormat:@"ENABLE_%@", bandstr];
        if(NULL == [prefs objectForKey:keystr] || factory_reset) {
            [self setBoolPref:keystr value:factory_band_enable];
        }
    }
    
    for(uint32_t i = 0; i < 5; i++) {
        bandstr = [NSString stringWithFormat:@"B%d", i+1];
        keystr = [NSString stringWithFormat:@"MB_COMP_DRIVE_%@", bandstr];
        if(NULL == [prefs objectForKey:keystr] || factory_reset) {
            [self setFloatPref:keystr value:factory_comp[i].drive];
        }
        keystr = [NSString stringWithFormat:@"MB_COMP_RELEASE_%@", bandstr];
        if(NULL == [prefs objectForKey:keystr] || factory_reset) {
            [self setFloatPref:keystr value:factory_comp[i].release];
        }
        keystr = [NSString stringWithFormat:@"MB_COMP_GATE_THRESH_%@", bandstr];
        if(NULL == [prefs objectForKey:keystr] || factory_reset) {
            [self setFloatPref:keystr value:factory_comp[i].gate_thresh];
        }
        keystr = [NSString stringWithFormat:@"MB_COMP_USE_COUPLING_%@", bandstr];
        if(NULL == [prefs objectForKey:keystr] || factory_reset) {
            [self setBoolPref:keystr value:factory_comp[i].use_coupling];
        }
        keystr = [NSString stringWithFormat:@"MB_COMP_COUPLING_%@", bandstr];
        if(NULL == [prefs objectForKey:keystr] || factory_reset) {
            [self setFloatPref:keystr value:factory_comp[i].coupling];
        }
        keystr = [NSString stringWithFormat:@"MB_COMP_WINDOW_SIZE_%@", bandstr];
        if(NULL == [prefs objectForKey:keystr] || factory_reset) {
            [self setFloatPref:keystr value:factory_comp[i].window_size];
        }
        keystr = [NSString stringWithFormat:@"MB_COMP_WINDOW_RELEASE_%@", bandstr];
        if(NULL == [prefs objectForKey:keystr] || factory_reset) {
            [self setFloatPref:keystr value:factory_comp[i].window_release];
        }
        keystr = [NSString stringWithFormat:@"MB_COMP_RATIO_%@", bandstr];
        if(NULL == [prefs objectForKey:keystr] || factory_reset) {
            [self setFloatPref:keystr value:factory_comp[i].ratio];
        }
        keystr = [NSString stringWithFormat:@"MB_COMP_IDLE_GAIN_%@", bandstr];
        if(NULL == [prefs objectForKey:keystr] || factory_reset) {
            [self setFloatPref:keystr value:factory_comp[i].idle_gain];
        }
        keystr = [NSString stringWithFormat:@"MB_COMP_ATTACK_%@", bandstr];
        if(NULL == [prefs objectForKey:keystr] || factory_reset) {
            [self setFloatPref:keystr value:factory_comp[i].attack];
        }
        keystr = [NSString stringWithFormat:@"MB_COMP_POST_GAIN_%@", bandstr];
        if(NULL == [prefs objectForKey:keystr] || factory_reset) {
            [self setFloatPref:keystr value:factory_comp[i].post_gain];
        }
        /* LIMITERS */
        keystr = [NSString stringWithFormat:@"MB_LIM_DRIVE_%@", bandstr];
        if(NULL == [prefs objectForKey:keystr] || factory_reset) {
            [self setFloatPref:keystr value:factory_lim[i].drive];
        }
        keystr = [NSString stringWithFormat:@"MB_LIM_RELEASE_%@", bandstr];
        if(NULL == [prefs objectForKey:keystr] || factory_reset) {
            [self setFloatPref:keystr value:factory_lim[i].release];
        }
        keystr = [NSString stringWithFormat:@"MB_LIM_GATE_THRESH_%@", bandstr];
        if(NULL == [prefs objectForKey:keystr] || factory_reset) {
            [self setFloatPref:keystr value:factory_lim[i].gate_thresh];
        }
        keystr = [NSString stringWithFormat:@"MB_LIM_USE_COUPLING_%@", bandstr];
        if(NULL == [prefs objectForKey:keystr] || factory_reset) {
            [self setBoolPref:keystr value:factory_lim[i].use_coupling];
        }
        keystr = [NSString stringWithFormat:@"MB_LIM_WINDOW_SIZE_%@", bandstr];
        if(NULL == [prefs objectForKey:keystr] || factory_reset) {
            [self setFloatPref:keystr value:factory_lim[i].window_size];
        }
        keystr = [NSString stringWithFormat:@"MB_LIM_WINDOW_RELEASE_%@", bandstr];
        if(NULL == [prefs objectForKey:keystr] || factory_reset) {
            [self setFloatPref:keystr value:factory_lim[i].window_release];
        }
        keystr = [NSString stringWithFormat:@"MB_LIM_COUPLING_%@", bandstr];
        if(NULL == [prefs objectForKey:keystr] || factory_reset) {
            [self setFloatPref:keystr value:factory_lim[i].coupling];
        }
        keystr = [NSString stringWithFormat:@"MB_LIM_RATIO_%@", bandstr];
        if(NULL == [prefs objectForKey:keystr] || factory_reset) {
            [self setFloatPref:keystr value:factory_lim[i].ratio];
        }
        keystr = [NSString stringWithFormat:@"MB_LIM_IDLE_GAIN_%@", bandstr];
        if(NULL == [prefs objectForKey:keystr] || factory_reset) {
            [self setFloatPref:keystr value:factory_lim[i].idle_gain];
        }
        keystr = [NSString stringWithFormat:@"MB_LIM_ATTACK_%@", bandstr];
        if(NULL == [prefs objectForKey:keystr] || factory_reset) {
            [self setFloatPref:keystr value:factory_lim[i].attack];
        }
        keystr = [NSString stringWithFormat:@"MB_LIM_POST_GAIN_%@", bandstr];
        if(NULL == [prefs objectForKey:keystr] || factory_reset) {
            [self setFloatPref:keystr value:factory_lim[i].post_gain];
        }
    }
    
}

-(IBAction) factory_reset:(NSButton*)sender {
    [self setupPrefsIfNeeded:YES];
    [self read_prefs];
}

- (void)controlTextDidChange:(NSNotification *)obj {
    NSLog(@"Control text changed.");
}

@end
