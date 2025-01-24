//
//  MultibandObjectController.m
//  MacSmoov
//
//  Created by Paul Zaremba on 7/6/24.
//

#import <Foundation/Foundation.h>
#import "MultibandObjectController.h"

@interface MultibandObjectController () {
}

@end

@implementation MultibandObjectController

-(id) init {
    self = [super init];
    
    if(self) {
        _b1_dict = [[NSMutableDictionary alloc] init];
        _b2_dict = [[NSMutableDictionary alloc] init];
        _b3_dict = [[NSMutableDictionary alloc] init];
        _b4_dict = [[NSMutableDictionary alloc] init];
        _b5_dict = [[NSMutableDictionary alloc] init];
        _the_dict = _b1_dict;
    }
    
    return self;
}

-(void) selectBand:(uint8_t) band {
    [self willChangeValueForKey:@"the_dict"];
    
    switch(band) {
        case 1:
            _the_dict = _b1_dict;
            break;
        case 2:
            _the_dict = _b2_dict;
            break;
        case 3:
            _the_dict = _b3_dict;
            break;
        case 4:
            _the_dict = _b4_dict;
            break;
        case 5:
            _the_dict = _b5_dict;
            break;
        default:
            NSLog(@"Bad band %d selected", band);
    }
    
    [self didChangeValueForKey:@"the_dict"];
}

-(MULTIBAND_PARAMS) getAllSettings {
    MULTIBAND_PARAMS params;
    
    params.comp_params[0].target = [[_b1_dict valueForKey:@"target"] floatValue];
    params.comp_params[0].release = [[_b1_dict valueForKey:@"release"] floatValue];
    params.comp_params[0].thresh = [[_b1_dict valueForKey:@"gate_thresh"] floatValue];
    params.comp_params[0].ratio = [[_b1_dict valueForKey:@"ratio"] floatValue];
    params.comp_params[0].attack = [[_b1_dict valueForKey:@"attack"] floatValue];
     
    return params;
}

-(void) setAllSettings:(MULTIBAND_PARAMS) settings {
    //_b1_dict[@"enabled"] = [NSNumber numberWithBool:settings.comp_params[0].enabled];
    //_b1_dict[@"solo"] = [NSNumber numberWithBool:settings.comp_params[0].solo];
    //_b1_dict[@"mute"] = [NSNumber numberWithBool:settings.comp_params[0].mute];
    //_b1_dict[@"drive"] = [NSNumber numberWithFloat:settings.comp_params[0].drive];
    _b1_dict[@"target"] = [NSNumber numberWithFloat:settings.comp_params[0].target];
    _b1_dict[@"release"] = [NSNumber numberWithFloat:settings.comp_params[0].release];
    _b1_dict[@"gate_thresh"] = [NSNumber numberWithFloat:settings.comp_params[0].thresh];
    //_b1_dict[@"use_coupling"] = [NSNumber numberWithBool:settings.comp_params[0].use_coupling];
    //_b1_dict[@"coupling"] = [NSNumber numberWithFloat:settings.comp_params[0].coupling];
    //_b1_dict[@"window_size"] = [NSNumber numberWithFloat:settings.comp_params[0].window_size];
    //_b1_dict[@"window_release"] = [NSNumber numberWithFloat:settings.comp_params[0].window_release];
    _b1_dict[@"ratio"] = [NSNumber numberWithFloat:settings.comp_params[0].ratio];
    //_b1_dict[@"idle_gain"] = [NSNumber numberWithFloat:settings.comp_params[0].idle_gain];
    _b1_dict[@"attack"] = [NSNumber numberWithFloat:settings.comp_params[0].attack];
    //_b1_dict[@"post_gain"] = [NSNumber numberWithFloat:settings.comp_params[0].post_gain];
    
}

-(void) dumpSettings {
    NSLog(@"MULTIBAND SETTINGS DUMP");
    NSLog(@"BAND 1: %@", _b1_dict);
    NSLog(@"BAND 2: %@", _b2_dict);
    NSLog(@"BAND 3: %@", _b3_dict);
    NSLog(@"BAND 4: %@", _b4_dict);
    NSLog(@"BAND 5: %@", _b5_dict);
}

@end
