//
//  AppDelegate.h
//  MacSmoov
//
//  Created by Paul Zaremba on 3/31/23.
//

#import <Cocoa/Cocoa.h>
#import "SignalGeneratorViewController.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>

-(IBAction) showSignalGenerators:(id)sender;
-(IBAction) adjustGainMainOut:(id)sender;

-(void) openAudioFile:(NSURL*)fileUrl;

@end

