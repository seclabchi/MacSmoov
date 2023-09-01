//
//  MultibandControlsView.m
//  MacSmoov
//
//  Created by Paul Zaremba on 8/28/23.
//

#import "MultibandControlsView.h"

@interface MultibandControlsView () {
    NSUserDefaults* prefs;
    uint8_t current_band;
}
@end



@implementation MultibandControlsView

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do view setup here.
    NSLog(@"MultibandControlsView viewDidLoad");
    
    //default band is 1 on startup
    current_band = 1;
    
    //Read band enable settings
    NSControlStateValue bands_enabled[5];
    
    bands_enabled[0] = [prefs boolForKey:@"ENABLE_B1"];
    bands_enabled[0] = [prefs boolForKey:@"ENABLE_B2"];
    bands_enabled[0] = [prefs boolForKey:@"ENABLE_B3"];
    bands_enabled[0] = [prefs boolForKey:@"ENABLE_B4"];
    bands_enabled[0] = [prefs boolForKey:@"ENABLE_B5"];
    
    [_stepper_compressor_drive setFloatValue: [_compressor_drive floatValue]];
    [_stepper_compressor_release setFloatValue: [_compressor_release floatValue]];
    
}

-(IBAction) band_select:(NSButton*)sender {
    NSLog(@"Band %lu selected.", (long)[sender tag]);
    current_band = (long)[sender tag];
}

-(void) setPrefs:(NSUserDefaults*) defaults {
    prefs = defaults;
}
-(void)showPanel {
    NSPanel* panel = (NSPanel*)[[self view] window];
}

-(IBAction) value_changed:(NSTextField*)sender{
    switch([sender tag]) {
        case 0: //compressor_drive
            break;
        case 1: //compressor_release
            break;
    }
}

-(IBAction) stepper_changed:(NSStepper*)sender {
    switch([sender tag]) {
        case 0: //compressor_drive
            [_compressor_drive setFloatValue:[_stepper_compressor_drive floatValue]];
            break;
        case 1: //compressor_release
            [_compressor_release setFloatValue:[_stepper_compressor_release floatValue]];
            break;
            
    }
}

@end
