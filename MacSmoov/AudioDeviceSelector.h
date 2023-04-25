//
//  AudioDeviceSelector.h
//  MacSmoov
//
//  Created by Paul Zaremba on 4/8/23.
//

#import <Cocoa/Cocoa.h>
#import <AudioToolbox/AudioToolbox.h>

NS_ASSUME_NONNULL_BEGIN

@interface AudioDeviceSelector : NSViewController <NSComboBoxDelegate> {
    id delegate;
}

@property(strong) IBOutlet NSComboBox* input_device_combo;
@property(strong) IBOutlet NSComboBox* output_device_combo;

-(void) set_watcher_for_output_device_change:(id)object andSelector:(SEL)selector;
-(void) set_watcher_for_input_device_change:(id)object andSelector:(SEL)selector;
-(void)showPanel;
-(void) scanDevices;
-(NSString*) getInputDeviceUID;
-(NSString*) getOutputDeviceUID;
@end

NS_ASSUME_NONNULL_END
