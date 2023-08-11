//
//  AudioDeviceSelector.m
//  MacSmoov
//
//  Created by Paul Zaremba on 4/8/23.
//

#import "AudioDeviceSelector.h"
#import "AudioDevice.h"

@interface AudioDeviceSelector ()
@property (strong) IBOutlet NSPanel *audio_device_selector_panel;
@property (strong) IBOutlet NSComboBox* inputDeviceComboBox;
@property (strong) IBOutlet NSComboBox* outputDeviceComboBox;

@end

@implementation AudioDeviceSelector {
    NSMutableDictionary* input_device_dict;
    NSMutableDictionary* output_device_dict;
    NSNumber* cur_input_device_id;
    NSNumber* cur_output_device_id;
    id output_device_watcher_object;
    SEL output_device_watcher_selector;
    id input_device_watcher_object;
    SEL input_device_watcher_selector;
    bool can_select;
}

- (id)initWithInputDevices:(NSMutableDictionary*)indevs outputDevices:(NSMutableDictionary*)outdevs {
    
    NSLog(@"Initing device selector panel...");
    
    self = [super init];
    
    if(self) {
        input_device_dict = indevs;
        output_device_dict = outdevs;
    }
    
    NSLog(@"Device selector panel inited.");
    
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do view setup here.
    self->can_select = FALSE;
    NSUserDefaultsController *prefs_controller = [NSUserDefaultsController sharedUserDefaultsController];
    NSUserDefaults *prefs = prefs_controller.defaults;
    cur_output_device_id = [prefs objectForKey:@"OUTPUT_DEVICE"];
    cur_input_device_id = [prefs objectForKey:@"INPUT_DEVICE"];
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
            
            NSArray* keys = [input_device_dict allKeys];
            AudioDevice* ad = [input_device_dict objectForKey:keys[selected_item]];
            cur_input_device_id = [NSNumber numberWithUnsignedInt: ad.device_id];
            [input_device_watcher_object performSelector:input_device_watcher_selector withObject:cur_input_device_id];
        }
        else {
            NSInteger selected_item = [combo indexOfSelectedItem];
            
            NSArray* keys = [output_device_dict allKeys];
            AudioDevice* ad = [output_device_dict objectForKey:keys[selected_item]];
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
}

-(void) scanDevices {
    
    self->can_select = FALSE;
    
    //[self.inputDeviceComboBox removeAllItems];
    //[self.outputDeviceComboBox removeAllItems];
    
    for(NSNumber* key in [output_device_dict allKeys]) {
        AudioDevice* ad = [output_device_dict objectForKey:key];
        //[self.outputDeviceComboBox addItemWithObjectValue:(__bridge NSString*)ad.device_name];
    }
    
    for(NSNumber* key in [input_device_dict allKeys]) {
        AudioDevice* ad = [input_device_dict objectForKey:key];
        //[self.inputDeviceComboBox addItemWithObjectValue:(__bridge NSString*)ad.device_name];
    }
    
    //[self.outputDeviceComboBox selectItemWithObjectValue:[output_device_dict objectForKey:cur_output_device_id]];
    //[self.inputDeviceComboBox selectItemWithObjectValue:[input_device_dict objectForKey:cur_input_device_id]];
    
    self->can_select = TRUE;
}

- (id)comboBox:(NSComboBox *)comboBox objectValueForItemAtIndex:(NSInteger)index {
    if([comboBox tag] == 0) {  //input combo
        NSArray* keys = [input_device_dict allKeys];
        AudioDevice* ad = [input_device_dict objectForKey:keys[index]];
        return (__bridge NSString*)ad.device_name;
    }
    else { //output combo
        NSArray* keys = [output_device_dict allKeys];
        AudioDevice* ad = [output_device_dict objectForKey:keys[index]];
        return (__bridge NSString*)ad.device_name;
    }
}

-(NSInteger) numberOfItemsInComboBox:(NSComboBox*)comboBox {
    if([comboBox tag] == 0) {  //input combo
        return [[input_device_dict allKeys] count];
    }
    else { //output combo
        return [[output_device_dict allKeys] count];
    }
}





@end
