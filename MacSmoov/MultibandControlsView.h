//
//  MultibandControlsView.h
//  MacSmoov
//
//  Created by Paul Zaremba on 8/28/23.
//

#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN

@interface MultibandControlsView : NSViewController

-(IBAction) band_select:(NSButton*)sender;

-(void) showPanel;

@end

NS_ASSUME_NONNULL_END
