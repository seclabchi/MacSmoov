//
//  AudioDeviceSelector.m
//  MacSmoov
//
//  Created by Paul Zaremba on 4/8/23.
//

#import "AudioDeviceSelector.h"

@interface AudioDeviceSelector ()


@end

@implementation AudioDeviceSelector {
    NSMutableArray<AudioDevice*>* input_devices;
    NSMutableArray<AudioDevice*>* output_devices;
    NSNumber* cur_input_device_id;
    NSNumber* cur_output_device_id;
    id output_device_watcher_object;
    SEL output_device_watcher_selector;
    id input_device_watcher_object;
    SEL input_device_watcher_selector;
    bool can_select;
}

- (id)initWithInputDevices:(NSMutableArray<AudioDevice*>*)indevs outputDevices:(NSMutableArray<AudioDevice*>*)outdevs {
    
    NSLog(@"Initing device selector panel...");
    
    self = [super init];
    
    if(self) {
        input_devices = [NSMutableArray arrayWithArray:indevs];
        output_devices = [NSMutableArray arrayWithArray:outdevs];
    }
    
    NSLog(@"Device selector panel inited.");
    
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do view setup here.
    self->can_select = FALSE;
    NSLog(@"AudioDeviceSelector viewDidLoad");
    [self scanDevices];
}

-(void) set_watcher_for_output_device_change:(id)object andSelector:(SEL)selector {
    output_device_watcher_object = object;
    output_device_watcher_selector = selector;
}

-(void) set_watcher_for_input_device_change:(id)object andSelector:(SEL)selector {
    input_device_watcher_object = object;
    input_device_watcher_selector = selector;
}

- (void)comboBoxSelectionDidChange:(NSNotification *)notification {

    if(TRUE == self->can_select) {
        NSComboBox* combo = [notification object];
        if([combo tag] == 0) {
            NSInteger selected_item = [combo indexOfSelectedItem];
            
            AudioDevice* ad = [input_devices objectAtIndex:selected_item];
            cur_input_device_id = [NSNumber numberWithUnsignedInt: ad.device_id];
            [input_device_watcher_object performSelector:input_device_watcher_selector withObject:cur_input_device_id];
        }
        else {
            NSInteger selected_item = [combo indexOfSelectedItem];
            
            AudioDevice* ad = [output_devices objectAtIndex:selected_item];
            cur_output_device_id = [NSNumber numberWithUnsignedInt: ad.device_id];
            [output_device_watcher_object performSelector:output_device_watcher_selector withObject:cur_output_device_id];
        }
    
    }
 
}

-(void)showPanel {
    NSPanel* panel = (NSPanel*)[[self view] window];
    [_audio_device_selector_panel setFloatingPanel:YES];
    [_audio_device_selector_panel orderFront:nil];
    _input_device_combo.delegate = self;
    _output_device_combo.delegate = self;
    self.output_device_combo.dataSource = self;
    self.input_device_combo.dataSource = self;
    [self scanDevices];
}

-(void) scanDevices {
    
    self->can_select = FALSE;
    
    //[self.input_device_combo removeAllItems];
    //[self.output_device_combo removeAllItems];
    
    
    
    //[self.outputDeviceComboBox selectItemWithObjectValue:[output_device_dict objectForKey:cur_output_device_id]];
    //[self.inputDeviceComboBox selectItemWithObjectValue:[input_device_dict objectForKey:cur_input_device_id]];
    
    self->can_select = TRUE;
}

- (NSString*)comboBox:(NSComboBox*)comboBox completedString:(NSString*)string {
    return @"";
}

- (NSUInteger) comboBox:(NSComboBox *) comboBox indexOfItemWithStringValue:(NSString *) string {
    NSUInteger idx = 0;
    
    for(uint32_t i = 0; i < input_devices.count; i++) {
        if([string compare: (__bridge NSString*) input_devices[i].device_name]) {
            return i;
        }
    }
    
    return idx;
}

- (id)comboBox:(NSComboBox *)comboBox objectValueForItemAtIndex:(NSInteger)index {
    if([comboBox tag] == 0) {  //input combo
        AudioDevice* ad = [input_devices objectAtIndex:index];
        return (__bridge NSString*)ad.device_name;
    }
    else { //output combo
        AudioDevice* ad = [output_devices objectAtIndex:index];
        return (__bridge NSString*)ad.device_name;
    }
}

-(NSInteger) numberOfItemsInComboBox:(NSComboBox*)comboBox {
    if([comboBox tag] == 0) {  //input combo
        return [input_devices count];
    } else { //output combo
        return [output_devices count];
    }
}





@end
