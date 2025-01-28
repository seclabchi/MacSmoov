//
//  MultibandObjectController.h
//  MacSmoov
//
//  Created by Paul Zaremba on 7/6/24.
//

#ifndef MultibandObjectController_h
#define MultibandObjectController_h

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import "core/core_common.h"

@interface TimeConstantValueTransformer : NSValueTransformer {
}

+ (Class)transformedValueClass;
+ (BOOL)allowsReverseTransformation;
- (id)transformedValue:(id)value;
- (id)reverseTransformedValue:(id)value;

@end

@interface MultibandObjectController : NSObjectController {
    
}
@property (readwrite, assign) bool enabled;

@property (readwrite, assign) float drive;
@property (readwrite, assign) float gate_thresh;

@property (readwrite, assign) bool band1_compressor_enabled;
@property (readwrite, assign) bool band2_compressor_enabled;
@property (readwrite, assign) bool band3_compressor_enabled;
@property (readwrite, assign) bool band4_compressor_enabled;
@property (readwrite, assign) bool band5_compressor_enabled;

@property (readwrite, assign) bool band1_limiter_enabled;
@property (readwrite, assign) bool band2_limiter_enabled;
@property (readwrite, assign) bool band3_limiter_enabled;
@property (readwrite, assign) bool band4_limiter_enabled;
@property (readwrite, assign) bool band5_limiter_enabled;

@property (readwrite, assign) bool band1_solo;
@property (readwrite, assign) bool band1_mute;
@property (readwrite, assign) bool band2_solo;
@property (readwrite, assign) bool band2_mute;
@property (readwrite, assign) bool band3_solo;
@property (readwrite, assign) bool band3_mute;
@property (readwrite, assign) bool band4_solo;
@property (readwrite, assign) bool band4_mute;
@property (readwrite, assign) bool band5_solo;
@property (readwrite, assign) bool band5_mute;

@property (readwrite, assign) float comp_b1_attack;
@property (readwrite, assign) float comp_b1_release;
@property (readwrite, assign) float comp_b1_ratio;
@property (readwrite, assign) float comp_b1_thresh;
@property (readwrite, assign) float comp_b1_target;

@property (readwrite, assign) float comp_b2_attack;
@property (readwrite, assign) float comp_b2_release;
@property (readwrite, assign) float comp_b2_ratio;
@property (readwrite, assign) float comp_b2_thresh;
@property (readwrite, assign) float comp_b2_target;

@property (readwrite, assign) float comp_b3_attack;
@property (readwrite, assign) float comp_b3_release;
@property (readwrite, assign) float comp_b3_ratio;
@property (readwrite, assign) float comp_b3_thresh;
@property (readwrite, assign) float comp_b3_target;

@property (readwrite, assign) float comp_b4_attack;
@property (readwrite, assign) float comp_b4_release;
@property (readwrite, assign) float comp_b4_ratio;
@property (readwrite, assign) float comp_b4_thresh;
@property (readwrite, assign) float comp_b4_target;

@property (readwrite, assign) float comp_b5_attack;
@property (readwrite, assign) float comp_b5_release;
@property (readwrite, assign) float comp_b5_ratio;
@property (readwrite, assign) float comp_b5_thresh;
@property (readwrite, assign) float comp_b5_target;

@property (readwrite, retain) NSMutableDictionary* b1_dict;
@property (readwrite, retain) NSMutableDictionary* b2_dict;
@property (readwrite, retain) NSMutableDictionary* b3_dict;
@property (readwrite, retain) NSMutableDictionary* b4_dict;
@property (readwrite, retain) NSMutableDictionary* b5_dict;
@property (readwrite, assign) NSMutableDictionary* the_dict;

-(id) init;
-(void) selectBand:(uint8_t) band;
-(MULTIBAND_PARAMS) getAllSettings;
-(void) setAllSettings:(MULTIBAND_PARAMS) settings;
-(void) dumpSettings;

@end

#endif /* MultibandObjectController_h */
