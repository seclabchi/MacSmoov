//
//  MultibandControlsView.h
//  MacSmoov
//
//  Created by Paul Zaremba on 8/28/23.
//

#import <Cocoa/Cocoa.h>
#import "core/core_common.h"
#import "MultibandObjectController.h"

NS_ASSUME_NONNULL_BEGIN

@protocol MultibandControlsDelegate <NSObject>
-(void) multiband_params_changed:(MULTIBAND_PARAMS) params;
@end

@interface MultibandControlsView : NSViewController {
 
}

@property (readwrite, retain) MultibandObjectController* mb_obj_ctrl;



@property (readwrite, assign) bool b1_enabled;
@property (readwrite, assign) bool b1_mute;
@property (readwrite, assign) bool b1_solo;
@property (readwrite, assign) int16_t b1_attack;
@property (readwrite, assign) int16_t b1_release;
@property (readwrite, assign) float b1_ratio;

@property (readwrite, assign) bool b2_enabled;
@property (readwrite, assign) bool b2_mute;
@property (readwrite, assign) bool b2_solo;
@property (readwrite, assign) int16_t b2_attack;
@property (readwrite, assign) int16_t b2_release;
@property (readwrite, assign) float b2_ratio;

@property (strong) IBOutlet NSButton* select_b1;
@property (strong) IBOutlet NSButton* select_b2;
@property (strong) IBOutlet NSButton* select_b3;
@property (strong) IBOutlet NSButton* select_b4;
@property (strong) IBOutlet NSButton* select_b5;

@property (strong) IBOutlet NSObjectController* objctrl;

@property (readwrite, assign) MULTIBAND_PARAMS mb_settings;

-(IBAction) setting_changed:(id) sender;
-(IBAction) band_select:(id)sender;

-(id) initWithSettings:(MULTIBAND_PARAMS) _settings delegate:(id)agc_delegate;

-(void) showPanel;

@end

NS_ASSUME_NONNULL_END
