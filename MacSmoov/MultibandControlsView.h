//
//  MultibandControlsView.h
//  MacSmoov
//
//  Created by Paul Zaremba on 8/28/23.
//

#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN

@interface MultibandControlsView : NSViewController {
    
}

@property (strong) IBOutlet NSStepper* stepper_compressor_drive;
@property (strong) IBOutlet NSStepper* stepper_compressor_release;
@property (strong) IBOutlet NSTextField* compressor_drive;
@property (strong) IBOutlet NSTextField* compressor_release;


-(IBAction) stepper_changed:(NSStepper*)sender;
-(IBAction) value_changed:(NSTextField*)sender;

-(IBAction) band_select:(NSButton*)sender;

-(void) setPrefs:(NSUserDefaults*) defaults;
-(void) showPanel;

@end

NS_ASSUME_NONNULL_END
