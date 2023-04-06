//
//  SignalGeneratorViewController.h
//  MacSmoov
//
//  Created by Paul Zaremba on 4/2/23.
//

#import <Cocoa/Cocoa.h>
#import "SignalGeneratorWrapper.h"

NS_ASSUME_NONNULL_BEGIN

@interface SignalGeneratorViewController : NSViewController
-(IBAction) functionTypeGen1:(id)sender;
-(IBAction) functionTypeGen2:(id)sender;
-(IBAction) functionTypeGen3:(id)sender;
-(IBAction) functionTypeGen4:(id)sender;
-(void) showPanel;
@end

#define BUFFER_SIZE 8192


NS_ASSUME_NONNULL_END
