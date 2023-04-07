//
//  SignalGeneratorViewController.h
//  MacSmoov
//
//  Created by Paul Zaremba on 4/2/23.
//

#import <Cocoa/Cocoa.h>
#import "SignalGenerator.h"

NS_ASSUME_NONNULL_BEGIN

@interface SignalGeneratorViewController : NSViewController
-(IBAction) functionTypeGen1:(id)sender;
-(IBAction) functionTypeGen2:(id)sender;
-(IBAction) functionTypeGen3:(id)sender;
-(IBAction) functionTypeGen4:(id)sender;

-(IBAction) gainAdjustGen1:(id)sender;
-(IBAction) gainAdjustGen2:(id)sender;
-(IBAction) gainAdjustGen3:(id)sender;
-(IBAction) gainAdjustGen4:(id)sender;

-(IBAction) startStopGen1:(id)sender;
-(IBAction) startStopGen2:(id)sender;
-(IBAction) startStopGen3:(id)sender;
-(IBAction) startStopGen4:(id)sender;

-(void) showPanel;
@end

#define BUFFER_SIZE 8192


NS_ASSUME_NONNULL_END
