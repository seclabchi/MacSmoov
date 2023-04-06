//
//  AppDelegate.h
//  MacSmoov
//
//  Created by Paul Zaremba on 3/31/23.
//

#import <Cocoa/Cocoa.h>
#import "SignalGeneratorViewController.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>

-(IBAction) generateSine400Hz:(id)sender;
-(IBAction) generateSine1kHz:(id)sender;
-(IBAction) generatePinkNoise(id)sender;
-(IBAction) showSignalGenerators:(id)sender;
-(IBAction) stopAudioGen:(id)sender;

-(IBAction) adjustVolume:(id)sender;

-(void) openAudioFile:(NSURL*)fileUrl;

@end

