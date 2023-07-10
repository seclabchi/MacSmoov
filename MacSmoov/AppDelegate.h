//
//  AppDelegate.h
//  MacSmoov
//
//  Created by Paul Zaremba on 3/31/23.
//

#import <Cocoa/Cocoa.h>
#import "SignalGeneratorViewController.h"
#import "AudioDeviceSelector.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (strong) IBOutlet NSLevelIndicator* level_main_in_l;
@property (strong) IBOutlet NSLevelIndicator* level_main_in_r;

-(void) output_device_changed:(NSString*)output_device;
-(void) input_device_changed:(NSString*)input_device;
-(IBAction) showSignalGenerators:(id)sender;
-(IBAction) showAudioDeviceSelector:(id)sender;
-(IBAction) adjustGainMainOut:(id)sender;


-(void) openAudioFile:(NSURL*)fileUrl;

@end

