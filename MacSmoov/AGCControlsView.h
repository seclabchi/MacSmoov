//
//  AGCControlsView.h
//  MacSmoov
//
//  Created by Paul Zaremba on 8/28/23.
//

#import <Cocoa/Cocoa.h>
#import "core/core_common.h"

NS_ASSUME_NONNULL_BEGIN

@protocol AGCControlsDelegate <NSObject>
-(void) agc_params_changed:(AGC_PARAMS) params;
@end

@interface AGCControlsView : NSViewController {
}

@property (readwrite, assign) BOOL enabled;
@property (readwrite, assign) float drive;
@property (readwrite, assign) float target;
@property (readwrite, assign) float release_master;
@property (readwrite, assign) float release_bass;
@property (readwrite, assign) float gate_thresh;
@property (readwrite, assign) float bass_coupling;
@property (readwrite, assign) float window_size;
@property (readwrite, assign) float window_release;
@property (readwrite, assign) float ratio;
@property (readwrite, assign) float bass_thresh;
@property (readwrite, assign) float idle_gain;
@property (readwrite, assign) float attack_master;
@property (readwrite, assign) float attack_bass;
@property (readwrite, assign) float post_gain;

@property (readwrite, assign) bool mute_lo;
@property (readwrite, assign) bool mute_hi;

@property (readwrite, assign) AGC_PARAMS agc_settings;

-(IBAction) factory_reset:(NSButton*)sender;

-(IBAction) setting_changed:(id) sender;

-(id) initWithSettings:(AGC_PARAMS) settings delegate:(id)agc_delegate;
-(void) showPanel;

- (void)controlTextDidChange:(NSNotification *)obj;

@end

NS_ASSUME_NONNULL_END
