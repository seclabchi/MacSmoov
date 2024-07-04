//
//  MultibandControlsView.h
//  MacSmoov
//
//  Created by Paul Zaremba on 8/28/23.
//

#import <Cocoa/Cocoa.h>
#import "core/core_common.h"

NS_ASSUME_NONNULL_BEGIN

@protocol MultibandControlsDelegate <NSObject>
-(void) multiband_params_changed:(MULTIBAND_PARAMS) params;
@end

@interface MultibandControlsView : NSViewController {
 
}

@property (readwrite, assign) bool b1_enabled;
@property (readwrite, assign) bool b1_mute;
@property (readwrite, assign) bool b1_solo;
@property (readwrite, assign) int8_t b1_drive;
@property (readwrite, assign) int16_t b1_attack;
@property (readwrite, assign) int16_t b1_release;
@property (readwrite, assign) float b1_ratio;
@property (readwrite, assign) int8_t b1_gate_thresh;

@property (strong) IBOutlet NSButton* select_b1;
@property (strong) IBOutlet NSButton* select_b2;
@property (strong) IBOutlet NSButton* select_b3;
@property (strong) IBOutlet NSButton* select_b4;
@property (strong) IBOutlet NSButton* select_b5;

@property (readwrite, assign) MULTIBAND_PARAMS mb_settings;

-(IBAction) setting_changed:(id) sender;
-(IBAction) band_select:(id)sender;

-(id) initWithSettings:(MULTIBAND_PARAMS) _settings delegate:(id)agc_delegate;

-(void) showPanel;

@end

NS_ASSUME_NONNULL_END
