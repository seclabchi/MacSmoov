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

-(void) populate_ui_elements:(uint32_t)current_band;

@end


@implementation MultibandControlsView


static const COMPRESSOR_PARAMS factory_comp1 = {
    .thresh = -18.0,
    .release = 0.100,
    .gate_thresh = 0.0,
    .ratio = 1.5,
    .attack = .050
};

COMPRESSOR_PARAMS factory_comp2 = {
    .thresh = -18.0,
    .release = 0.100,
    .gate_thresh = 0.0,
    .ratio = 1.5,
    .attack = .050
};

COMPRESSOR_PARAMS factory_comp3 = {
    .thresh = -18.0,
    .release = 0.100,
    .gate_thresh = 0.0,
    .ratio = 1.5,
    .attack = .050
};

COMPRESSOR_PARAMS factory_comp4 = {
    .thresh = -18.0,
    .release = 0.100,
    .gate_thresh = 0.0,
    .ratio = 1.5,
    .attack = .050
};

COMPRESSOR_PARAMS factory_comp5 = {
    .thresh = -18.0,
    .release = 0.100,
    .gate_thresh = 0.0,
    .ratio = 1.5,
    .attack = .050
};

/*===================================*/
/* LIMITERS                          */
/*===================================*/
COMPRESSOR_PARAMS factory_lim1 = {
    .thresh = -18.0,
    .release = 0.050,
    .gate_thresh = 0.0,
    .ratio = 20.0,
    .attack = 0.050,
};

COMPRESSOR_PARAMS factory_lim2 = {
    .thresh = -18.0,
    .release = 0.050,
    .gate_thresh = 0.0,
    .ratio = 20.0,
    .attack = 0.050,
};

COMPRESSOR_PARAMS factory_lim3 = {
    .thresh = -18.0,
    .release = 0.050,
    .gate_thresh = 0.0,
    .ratio = 20.0,
    .attack = 0.050,
};

COMPRESSOR_PARAMS factory_lim4 = {
    .thresh = -18.0,
    .release = 0.050,
    .gate_thresh = 0.0,
    .ratio = 20.0,
    .attack = 0.050,
};

COMPRESSOR_PARAMS factory_lim5 = {
    .thresh = -18.0,
    .release = 0.050,
    .gate_thresh = 0.0,
    .ratio = 20.0,
    .attack = 0.050,
};



-(id) initWithSettings:(MULTIBAND_PARAMS) _settings delegate:(id)mb_delegate {
    self = [super init];
    if(self) {
        COMPRESSOR_PARAMS factory_comp[5] = {
            factory_comp1,
            factory_comp2,
            factory_comp3,
            factory_comp4,
            factory_comp5
        };
        
        COMPRESSOR_PARAMS factory_lim[5] = {
            factory_lim1,
            factory_lim2,
            factory_lim3,
            factory_lim4,
            factory_lim5
        };

        MULTIBAND_PARAMS factory_mb = {
            .enabled = true,
            .drive = 0.0,
            .gate_thresh = -18.0f,
            .band1_solo = false,
            .band1_mute = false,
            .band2_solo = false,
            .band2_mute = false,
            .band3_solo = false,
            .band3_mute = false,
            .band4_solo = false,
            .band4_mute = false,
            .band5_solo = false,
            .band5_mute = false,
            .band1_compressor_enabled = true,
            .band2_compressor_enabled = true,
            .band3_compressor_enabled = true,
            .band4_compressor_enabled = true,
            .band5_compressor_enabled = true,
            .band1_limiter_enabled = true,
            .band2_limiter_enabled = true,
            .band3_limiter_enabled = true,
            .band4_limiter_enabled = true,
            .band5_limiter_enabled = true,
            .band34_coupling = 0.0f,
            .band45_coupling = 0.0f,
            .band32_coupling = 0.0f,
            .band23_coupling = 0.0f,
            .band21_coupling = 0.0f,
        };
        
        memcpy(factory_mb.comp_params, factory_comp, 5 * sizeof(COMPRESSOR_PARAMS));
        memcpy(factory_mb.lim_params, factory_lim, 5 * sizeof(COMPRESSOR_PARAMS));
        
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
    if([sender class] == [NSTextField class]) {
        NSTextField* textField = sender;
        NSLog(@"Text field setting changed: %@ with tag %ld to value %@", sender, (long)textField.tag, [[textField cell] stringValue]);
    }
    else if([sender class] == [NSButton class]) {
        NSButton* button = sender;
        NSLog(@"Button setting changed: %@ with tag %ld", sender, (long)button.tag);
    }
    else if([sender class] == [NSSlider class]) {
        NSSlider* slider = sender;
        NSLog(@"Slider setting changed: %@ with tag %ld", sender, (long)slider.tag);
    }
    else {
        NSLog(@"Unknown UI element caused a setting change: %@", sender);
    }
    
    //[_mb_obj_ctrl dumpSettings];
    _mb_settings = [_mb_obj_ctrl getAllSettings];
    [delegate multiband_params_changed:_mb_settings];

}

@end
