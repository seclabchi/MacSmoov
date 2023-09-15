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
-(void) band_enablement_changed:(NSControlStateValue[]) band_states;
@end

@interface MultibandControlsView : NSViewController {
    
}

@property (strong) IBOutlet NSStepper* stepper_compressor_drive;
@property (strong) IBOutlet NSStepper* stepper_compressor_release;
@property (strong) IBOutlet NSStepper* stepper_compressor_gate_thresh;
@property (strong) IBOutlet NSStepper* stepper_compressor_ratio;
@property (strong) IBOutlet NSStepper* stepper_compressor_idle_gain;
@property (strong) IBOutlet NSStepper* stepper_compressor_attack;

@property (strong) IBOutlet NSTextField* compressor_drive;
@property (strong) IBOutlet NSTextField* compressor_release;
@property (strong) IBOutlet NSTextField* compressor_gate_thresh;
@property (strong) IBOutlet NSTextField* compressor_ratio;
@property (strong) IBOutlet NSTextField* compressor_idle_gain;
@property (strong) IBOutlet NSTextField* compressor_attack;

@property (strong) IBOutlet NSStepper* stepper_limiter_drive;
@property (strong) IBOutlet NSStepper* stepper_limiter_release;
@property (strong) IBOutlet NSStepper* stepper_limiter_ratio;
@property (strong) IBOutlet NSStepper* stepper_limiter_idle_gain;
@property (strong) IBOutlet NSStepper* stepper_limiter_attack;
@property (strong) IBOutlet NSStepper* stepper_limiter_output_gain;

@property (strong) IBOutlet NSTextField* limiter_drive;
@property (strong) IBOutlet NSTextField* limiter_release;
@property (strong) IBOutlet NSTextField* limiter_ratio;
@property (strong) IBOutlet NSTextField* limiter_idle_gain;
@property (strong) IBOutlet NSTextField* limiter_attack;
@property (strong) IBOutlet NSTextField* limiter_output_gain;


@property (strong) IBOutlet NSButton* enable_b1;
@property (strong) IBOutlet NSButton* enable_b2;
@property (strong) IBOutlet NSButton* enable_b3;
@property (strong) IBOutlet NSButton* enable_b4;
@property (strong) IBOutlet NSButton* enable_b5;

-(IBAction) bandEnableChanged:(NSButton*)sender;

-(IBAction) stepper_changed_compressor:(NSStepper*)sender;
-(IBAction) value_changed_compressor:(NSTextField*)sender;
-(IBAction) stepper_changed_limiter:(NSStepper*)sender;
-(IBAction) value_changed_limiter:(NSTextField*)sender;

-(IBAction) band_select:(NSButton*)sender;

-(id) initWithPrefs:(NSUserDefaults*) defaults delegate:(id)mb_delegate;
-(void) setupPrefsIfNeeded:(BOOL)factory_reset;
-(void) init_defaults:(BOOL)force_factory;
-(void) showPanel;

@end

NS_ASSUME_NONNULL_END
