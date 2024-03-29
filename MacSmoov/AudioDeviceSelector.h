//
//  AudioDeviceSelector.h
//  MacSmoov
//
//  Created by Paul Zaremba on 4/8/23.
//

#import <Cocoa/Cocoa.h>
#import <AudioToolbox/AudioToolbox.h>

NS_ASSUME_NONNULL_BEGIN

@interface AudioDeviceSelector : NSViewController <NSComboBoxDelegate, NSComboBoxDataSource> {
    id delegate;
}

@property(strong) IBOutlet NSComboBox* input_device_combo;
@property(strong) IBOutlet NSComboBox* output_device_combo;

- (id)initWithInputDevices:(NSMutableDictionary*)indevs outputDevices:(NSMutableDictionary*)outdevs;
- (void) set_watcher_for_output_device_change:(id)object andSelector:(SEL)selector;
- (void) set_watcher_for_input_device_change:(id)object andSelector:(SEL)selector;
- (void)showPanel;
- (void) scanDevices;

- (id)comboBox:(NSComboBox *)comboBox objectValueForItemAtIndex:(NSInteger)index;
- (NSInteger) numberOfItemsInComboBox:(NSComboBox*)comboBox;

@end

NS_ASSUME_NONNULL_END
