//
//  MultibandObjectController.m
//  MacSmoov
//
//  Created by Paul Zaremba on 7/6/24.
//

#import <Foundation/Foundation.h>
#import "MultibandObjectController.h"

@implementation TimeConstantValueTransformer
+ (Class)transformedValueClass
{
    return [NSNumber class];
}

+ (BOOL)allowsReverseTransformation
{
    return YES;
}

- (id)transformedValue:(id)value
{
    float timeconst_input;
    float timeconst_slider_value;
 
    if (value == nil) return nil;
 
    // Attempt to get a reasonable value from the
    // value object.
    if ([value respondsToSelector: @selector(floatValue)]) {
    // handles NSString and NSNumber
        timeconst_input = [value floatValue];
    } else {
        [NSException raise: NSInternalInconsistencyException
                    format: @"Value (%@) does not respond to -floatValue.",
        [value class]];
    }
 
    // calculate slider value
    timeconst_slider_value = timeconst_input * 1000.0f;
 
    return [NSNumber numberWithFloat: timeconst_slider_value];
}

- (id)reverseTransformedValue:(id)value
{
    float timeconst_slider_value;
    float timeconst_output;
 
    if (value == nil) return nil;
 
    // Attempt to get a reasonable value from the
    // value object.
    if ([value respondsToSelector: @selector(floatValue)]) {
    // handles NSString and NSNumber
        timeconst_slider_value = [value floatValue];
    } else {
        [NSException raise: NSInternalInconsistencyException
                    format: @"Value (%@) does not respond to -floatValue.",
        [value class]];
    }
 
    // calculate actual timeconst value
    timeconst_output = timeconst_slider_value / 1000.0f;
 
    return [NSNumber numberWithFloat: timeconst_output];
}

@end

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
    
    params.enabled = _multiband_enabled;
    params.drive = _drive;
    params.gate_thresh = _gate_thresh;
    params.limiters_enabled = _limiters_enabled;
    params.master_post_gain = _master_post_gain;
    
    params.band1_compressor_enabled = _band1_compressor_enabled;
    params.band2_compressor_enabled = _band2_compressor_enabled;
    params.band3_compressor_enabled = _band3_compressor_enabled;
    params.band4_compressor_enabled = _band4_compressor_enabled;
    params.band5_compressor_enabled = _band5_compressor_enabled;
    
    params.band34_coupling = _band34_coupling;
    params.band45_coupling = _band45_coupling;
    params.band32_coupling = _band32_coupling;
    params.band23_coupling = _band23_coupling;
    params.band21_coupling = _band21_coupling;
    
    params.band1_limiter_enabled = [[_b1_dict valueForKey:@"lim_enabled"] boolValue];
    params.band2_limiter_enabled = [[_b2_dict valueForKey:@"lim_enabled"] boolValue];
    params.band3_limiter_enabled = [[_b3_dict valueForKey:@"lim_enabled"] boolValue];
    params.band4_limiter_enabled = [[_b4_dict valueForKey:@"lim_enabled"] boolValue];
    params.band5_limiter_enabled = [[_b5_dict valueForKey:@"lim_enabled"] boolValue];
    
    params.band1_solo = _band1_solo;
    params.band1_mute = _band1_mute;
    params.band2_solo = _band2_solo;
    params.band2_mute = _band2_mute;
    params.band3_solo = _band3_solo;
    params.band3_mute = _band3_mute;
    params.band4_solo = _band4_solo;
    params.band4_mute = _band4_mute;
    params.band5_solo = _band5_solo;
    params.band5_mute = _band5_mute;
    
    params.band1_comp_lim_offset = [[_b1_dict valueForKey:@"comp_offset"] floatValue];
    params.band2_comp_lim_offset = [[_b2_dict valueForKey:@"comp_offset"] floatValue];
    params.band3_comp_lim_offset = [[_b3_dict valueForKey:@"comp_offset"] floatValue];
    params.band4_comp_lim_offset = [[_b4_dict valueForKey:@"comp_offset"] floatValue];
    params.band5_comp_lim_offset = [[_b5_dict valueForKey:@"comp_offset"] floatValue];
    
    params.comp_params[0].release = [[_b1_dict valueForKey:@"comp_release"] floatValue];
    params.comp_params[0].ratio = [[_b1_dict valueForKey:@"comp_ratio"] floatValue];
    params.comp_params[0].attack = [[_b1_dict valueForKey:@"comp_attack"] floatValue];
    params.comp_params[0].target = [[_b1_dict valueForKey:@"comp_target"] floatValue];
    params.comp_params[0].thresh = [[_b1_dict valueForKey:@"comp_thresh"] floatValue];
    params.lim_params[0].release = [[_b1_dict valueForKey:@"lim_release"] floatValue];
    params.lim_params[0].ratio = [[_b1_dict valueForKey:@"lim_ratio"] floatValue];
    params.lim_params[0].attack = [[_b1_dict valueForKey:@"lim_attack"] floatValue];
    params.lim_params[0].target = [[_b1_dict valueForKey:@"lim_target"] floatValue];
    params.lim_params[0].thresh = [[_b1_dict valueForKey:@"lim_thresh"] floatValue];
    
    params.comp_params[1].release = [[_b2_dict valueForKey:@"comp_release"] floatValue];
    params.comp_params[1].ratio = [[_b2_dict valueForKey:@"comp_ratio"] floatValue];
    params.comp_params[1].attack = [[_b2_dict valueForKey:@"comp_attack"] floatValue];
    params.comp_params[1].target = [[_b2_dict valueForKey:@"comp_target"] floatValue];
    params.comp_params[1].thresh = [[_b2_dict valueForKey:@"comp_thresh"] floatValue];
    params.lim_params[1].release = [[_b2_dict valueForKey:@"lim_release"] floatValue];
    params.lim_params[1].ratio = [[_b2_dict valueForKey:@"lim_ratio"] floatValue];
    params.lim_params[1].attack = [[_b2_dict valueForKey:@"lim_attack"] floatValue];
    params.lim_params[1].target = [[_b2_dict valueForKey:@"lim_target"] floatValue];
    params.lim_params[1].thresh = [[_b2_dict valueForKey:@"lim_thresh"] floatValue];
    
    params.comp_params[2].release = [[_b3_dict valueForKey:@"comp_release"] floatValue];
    params.comp_params[2].ratio = [[_b3_dict valueForKey:@"comp_ratio"] floatValue];
    params.comp_params[2].attack = [[_b3_dict valueForKey:@"comp_attack"] floatValue];
    params.comp_params[2].target = [[_b3_dict valueForKey:@"comp_target"] floatValue];
    params.comp_params[2].thresh = [[_b3_dict valueForKey:@"comp_thresh"] floatValue];
    params.lim_params[2].release = [[_b3_dict valueForKey:@"lim_release"] floatValue];
    params.lim_params[2].ratio = [[_b3_dict valueForKey:@"lim_ratio"] floatValue];
    params.lim_params[2].attack = [[_b3_dict valueForKey:@"lim_attack"] floatValue];
    params.lim_params[2].target = [[_b3_dict valueForKey:@"lim_target"] floatValue];
    params.lim_params[2].thresh = [[_b3_dict valueForKey:@"lim_thresh"] floatValue];
    
    params.comp_params[3].release = [[_b4_dict valueForKey:@"comp_release"] floatValue];
    params.comp_params[3].ratio = [[_b4_dict valueForKey:@"comp_ratio"] floatValue];
    params.comp_params[3].attack = [[_b4_dict valueForKey:@"comp_attack"] floatValue];
    params.comp_params[3].target = [[_b4_dict valueForKey:@"comp_target"] floatValue];
    params.comp_params[3].thresh = [[_b4_dict valueForKey:@"comp_thresh"] floatValue];
    params.lim_params[3].release = [[_b4_dict valueForKey:@"lim_release"] floatValue];
    params.lim_params[3].ratio = [[_b4_dict valueForKey:@"lim_ratio"] floatValue];
    params.lim_params[3].attack = [[_b4_dict valueForKey:@"lim_attack"] floatValue];
    params.lim_params[3].target = [[_b4_dict valueForKey:@"lim_target"] floatValue];
    params.lim_params[3].thresh = [[_b4_dict valueForKey:@"lim_thresh"] floatValue];
    
    params.comp_params[4].release = [[_b5_dict valueForKey:@"comp_release"] floatValue];
    params.comp_params[4].ratio = [[_b5_dict valueForKey:@"comp_ratio"] floatValue];
    params.comp_params[4].attack = [[_b5_dict valueForKey:@"comp_attack"] floatValue];
    params.comp_params[4].target = [[_b5_dict valueForKey:@"comp_target"] floatValue];
    params.comp_params[4].thresh = [[_b5_dict valueForKey:@"comp_thresh"] floatValue];
    params.lim_params[4].release = [[_b5_dict valueForKey:@"lim_release"] floatValue];
    params.lim_params[4].ratio = [[_b5_dict valueForKey:@"lim_ratio"] floatValue];
    params.lim_params[4].attack = [[_b5_dict valueForKey:@"lim_attack"] floatValue];
    params.lim_params[4].target = [[_b5_dict valueForKey:@"lim_target"] floatValue];
    params.lim_params[4].thresh = [[_b5_dict valueForKey:@"lim_thresh"] floatValue];
    
    /* Gotta recalculate the limiter thresholds per band with offset
     * TODO: FIGURE OUT THE SETTINGS - one single place to compute everything
     */
    
    params.lim_params[0].thresh = params.comp_params[0].target + params.band1_comp_lim_offset;
    params.lim_params[1].thresh = params.comp_params[1].target + params.band2_comp_lim_offset;
    params.lim_params[2].thresh = params.comp_params[2].target + params.band3_comp_lim_offset;
    params.lim_params[3].thresh = params.comp_params[3].target + params.band4_comp_lim_offset;
    params.lim_params[4].thresh = params.comp_params[4].target + params.band5_comp_lim_offset;
     
    return params;
}

-(void) setAllSettings:(MULTIBAND_PARAMS) settings {
    _multiband_enabled = settings.enabled;
    _drive = settings.drive;
    _gate_thresh = settings.gate_thresh;
    _limiters_enabled = settings.limiters_enabled;
    _master_post_gain = settings.master_post_gain;
    
    _band1_compressor_enabled = settings.band1_compressor_enabled;
    _band2_compressor_enabled = settings.band2_compressor_enabled;
    _band3_compressor_enabled = settings.band3_compressor_enabled;
    _band4_compressor_enabled = settings.band4_compressor_enabled;
    _band5_compressor_enabled = settings.band5_compressor_enabled;
    
    _band34_coupling = settings.band34_coupling;
    _band45_coupling = settings.band45_coupling;
    _band32_coupling = settings.band32_coupling;
    _band23_coupling = settings.band23_coupling;
    _band21_coupling = settings.band21_coupling;
    
    _b1_dict[@"lim_enabled"] = [NSNumber numberWithBool:settings.band1_limiter_enabled];
    _b2_dict[@"lim_enabled"] = [NSNumber numberWithBool:settings.band2_limiter_enabled];
    _b3_dict[@"lim_enabled"] = [NSNumber numberWithBool:settings.band3_limiter_enabled];
    _b4_dict[@"lim_enabled"] = [NSNumber numberWithBool:settings.band4_limiter_enabled];
    _b5_dict[@"lim_enabled"] = [NSNumber numberWithBool:settings.band5_limiter_enabled];
    
    _band1_solo = settings.band1_solo;
    _band1_mute = settings.band1_mute;
    _band2_solo = settings.band2_solo;
    _band2_mute = settings.band2_mute;
    _band3_solo = settings.band3_solo;
    _band3_mute = settings.band3_mute;
    _band4_solo = settings.band4_solo;
    _band4_mute = settings.band4_mute;
    _band5_solo = settings.band5_solo;
    _band5_mute = settings.band5_mute;
    
    _b1_dict[@"comp_offset"] = [NSNumber numberWithFloat:settings.band1_comp_lim_offset];
    _b2_dict[@"comp_offset"] = [NSNumber numberWithFloat:settings.band2_comp_lim_offset];
    _b3_dict[@"comp_offset"] = [NSNumber numberWithFloat:settings.band3_comp_lim_offset];
    _b4_dict[@"comp_offset"] = [NSNumber numberWithFloat:settings.band4_comp_lim_offset];
    _b5_dict[@"comp_offset"] = [NSNumber numberWithFloat:settings.band5_comp_lim_offset];
    
    _b1_dict[@"comp_release"] = [NSNumber numberWithFloat:settings.comp_params[0].release];
    _b1_dict[@"comp_ratio"] = [NSNumber numberWithFloat:settings.comp_params[0].ratio];
    _b1_dict[@"comp_attack"] = [NSNumber numberWithFloat:settings.comp_params[0].attack];
    _b1_dict[@"comp_target"] = [NSNumber numberWithFloat:settings.comp_params[0].target];
    _b1_dict[@"comp_thresh"] = [NSNumber numberWithFloat:settings.comp_params[0].thresh];
    _b1_dict[@"lim_release"] = [NSNumber numberWithFloat:settings.lim_params[0].release];
    _b1_dict[@"lim_ratio"] = [NSNumber numberWithFloat:settings.lim_params[0].ratio];
    _b1_dict[@"lim_attack"] = [NSNumber numberWithFloat:settings.lim_params[0].attack];
    _b1_dict[@"lim_target"] = [NSNumber numberWithFloat:settings.lim_params[0].target];
    _b1_dict[@"lim_thresh"] = [NSNumber numberWithFloat:settings.lim_params[0].thresh];
    
    _b2_dict[@"comp_release"] = [NSNumber numberWithFloat:settings.comp_params[1].release];
    _b2_dict[@"comp_ratio"] = [NSNumber numberWithFloat:settings.comp_params[1].ratio];
    _b2_dict[@"comp_attack"] = [NSNumber numberWithFloat:settings.comp_params[1].attack];
    _b2_dict[@"comp_target"] = [NSNumber numberWithFloat:settings.comp_params[1].target];
    _b2_dict[@"comp_thresh"] = [NSNumber numberWithFloat:settings.comp_params[1].thresh];
    _b2_dict[@"lim_release"] = [NSNumber numberWithFloat:settings.lim_params[1].release];
    _b2_dict[@"lim_ratio"] = [NSNumber numberWithFloat:settings.lim_params[1].ratio];
    _b2_dict[@"lim_attack"] = [NSNumber numberWithFloat:settings.lim_params[1].attack];
    _b2_dict[@"lim_target"] = [NSNumber numberWithFloat:settings.lim_params[1].target];
    _b2_dict[@"lim_thresh"] = [NSNumber numberWithFloat:settings.lim_params[1].thresh];
    
    _b3_dict[@"comp_release"] = [NSNumber numberWithFloat:settings.comp_params[2].release];
    _b3_dict[@"comp_ratio"] = [NSNumber numberWithFloat:settings.comp_params[2].ratio];
    _b3_dict[@"comp_attack"] = [NSNumber numberWithFloat:settings.comp_params[2].attack];
    _b3_dict[@"comp_target"] = [NSNumber numberWithFloat:settings.comp_params[2].target];
    _b3_dict[@"comp_thresh"] = [NSNumber numberWithFloat:settings.comp_params[2].thresh];
    _b3_dict[@"lim_release"] = [NSNumber numberWithFloat:settings.lim_params[2].release];
    _b3_dict[@"lim_ratio"] = [NSNumber numberWithFloat:settings.lim_params[2].ratio];
    _b3_dict[@"lim_attack"] = [NSNumber numberWithFloat:settings.lim_params[2].attack];
    _b3_dict[@"lim_target"] = [NSNumber numberWithFloat:settings.lim_params[2].target];
    _b3_dict[@"lim_thresh"] = [NSNumber numberWithFloat:settings.lim_params[2].thresh];
    
    _b4_dict[@"comp_release"] = [NSNumber numberWithFloat:settings.comp_params[3].release];
    _b4_dict[@"comp_ratio"] = [NSNumber numberWithFloat:settings.comp_params[3].ratio];
    _b4_dict[@"comp_attack"] = [NSNumber numberWithFloat:settings.comp_params[3].attack];
    _b4_dict[@"comp_target"] = [NSNumber numberWithFloat:settings.comp_params[3].target];
    _b4_dict[@"comp_thresh"] = [NSNumber numberWithFloat:settings.comp_params[3].thresh];
    _b4_dict[@"lim_release"] = [NSNumber numberWithFloat:settings.lim_params[3].release];
    _b4_dict[@"lim_ratio"] = [NSNumber numberWithFloat:settings.lim_params[3].ratio];
    _b4_dict[@"lim_attack"] = [NSNumber numberWithFloat:settings.lim_params[3].attack];
    _b4_dict[@"lim_target"] = [NSNumber numberWithFloat:settings.lim_params[3].target];
    _b4_dict[@"lim_thresh"] = [NSNumber numberWithFloat:settings.lim_params[3].thresh];

    _b5_dict[@"comp_release"] = [NSNumber numberWithFloat:settings.comp_params[4].release];
    _b5_dict[@"comp_ratio"] = [NSNumber numberWithFloat:settings.comp_params[4].ratio];
    _b5_dict[@"comp_attack"] = [NSNumber numberWithFloat:settings.comp_params[4].attack];
    _b5_dict[@"comp_target"] = [NSNumber numberWithFloat:settings.comp_params[4].target];
    _b5_dict[@"comp_thresh"] = [NSNumber numberWithFloat:settings.comp_params[4].thresh];
    _b5_dict[@"lim_release"] = [NSNumber numberWithFloat:settings.lim_params[4].release];
    _b5_dict[@"lim_ratio"] = [NSNumber numberWithFloat:settings.lim_params[4].ratio];
    _b5_dict[@"lim_attack"] = [NSNumber numberWithFloat:settings.lim_params[4].attack];
    _b5_dict[@"lim_target"] = [NSNumber numberWithFloat:settings.lim_params[4].target];
    _b5_dict[@"lim_thresh"] = [NSNumber numberWithFloat:settings.lim_params[4].thresh];
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
