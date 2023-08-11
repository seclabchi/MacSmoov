//
//  AppDelegate.h
//  MacSmoov
//
//  Created by Paul Zaremba on 3/31/23.
//

#import <Cocoa/Cocoa.h>
#import "SignalGeneratorViewController.h"
#import "AudioDeviceSelector.h"
#import "LevelMeter.h"
#import "OSXAudioInterface.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (strong) IBOutlet LevelMeter* level_main_in;

@property (nonatomic, strong) OSXAudioInterface * sysaudio;

-(void) output_device_changed:(NSNumber*)output_device;
-(void) input_device_changed:(NSNumber*)input_device;
-(IBAction) showSignalGenerators:(id)sender;
-(IBAction) showAudioDeviceSelector:(id)sender;
-(IBAction) adjustGainMainIn:(id)sender;

- (void) queryMeterLevels:(id)param;
-(void) openAudioFile:(NSURL*)fileUrl;

@end

