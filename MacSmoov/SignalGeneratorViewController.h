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
-(IBAction) setFunctionType400Hz:(id)sender;
-(IBAction) setFunctionType1kHz:(id)sender;
-(IBAction) setFunctionTypeCustom:(id)sender;
-(IBAction) setFunctionTypeNoise:(id)sender;

-(IBAction) setGain400Hz:(id)sender;
-(IBAction) setGain1kHz:(id)sender;
-(IBAction) setGainCustom:(id)sender;
-(IBAction) setGainNoise:(id)sender;

-(IBAction) startStop400Hz:(id)sender;
-(IBAction) startStop1kHz:(id)sender;
-(IBAction) startStopCustom:(id)sender;
-(IBAction) startStopNoise:(id)sender;

-(void) showPanel;
@end

#define BUFFER_SIZE 8192


NS_ASSUME_NONNULL_END
