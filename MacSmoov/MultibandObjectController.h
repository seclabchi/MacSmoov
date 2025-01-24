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


@interface MultibandObjectController : NSObjectController {
    
}

@property (readwrite, assign) bool b1_enabled;
@property (readwrite, assign) bool b1_mute;
@property (readwrite, assign) bool b1_solo;
@property (readwrite, assign) float b1_drive;
@property (readwrite, assign) float b1_attack;
@property (readwrite, assign) float b1_release;
@property (readwrite, assign) float b1_ratio;
@property (readwrite, assign) float b1_gate_thresh;

@property (readwrite, assign) bool b2_enabled;
@property (readwrite, assign) bool b2_mute;
@property (readwrite, assign) bool b2_solo;
@property (readwrite, assign) float b2_drive;
@property (readwrite, assign) float b2_attack;
@property (readwrite, assign) float b2_release;
@property (readwrite, assign) float b2_ratio;
@property (readwrite, assign) float b2_gate_thresh;

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
