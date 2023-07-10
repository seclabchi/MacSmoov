//
//  AudioDeviceSelector.m
//  MacSmoov
//
//  Created by Paul Zaremba on 4/8/23.
//

#import "AudioDeviceSelector.h"

@interface AudioDeviceSelector ()
@property (strong) IBOutlet NSPanel *audio_device_selector_panel;
@property (strong) IBOutlet NSComboBox* inputDeviceComboBox;
@property (strong) IBOutlet NSComboBox* outputDeviceComboBox;

@end

@implementation AudioDeviceSelector {
    NSMutableDictionary* audio_output_device_dict;
    NSMutableDictionary* audio_input_device_dict;
    NSString* cur_output_device_uid;
    NSString* cur_input_device_uid;
    id output_device_watcher_object;
    SEL output_device_watcher_selector;
    id input_device_watcher_object;
    SEL input_device_watcher_selector;
    bool can_select;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do view setup here.
    self->can_select = FALSE;
    audio_output_device_dict = [[NSMutableDictionary alloc] init];
    audio_input_device_dict = [[NSMutableDictionary alloc] init];
    NSUserDefaultsController *prefs_controller = [NSUserDefaultsController sharedUserDefaultsController];
    NSUserDefaults *prefs = prefs_controller.defaults;
    cur_output_device_uid = [prefs objectForKey:@"OUTPUT_DEVICE"];
    cur_input_device_uid = [prefs objectForKey:@"INPUT_DEVICE"];
    NSLog(@"AudioDeviceSelector viewDidLoad");
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
            NSLog(@"Input device changed to %@", [combo objectValueOfSelectedItem]);
            cur_input_device_uid = [audio_input_device_dict allKeysForObject:[combo objectValueOfSelectedItem]][0];
            [input_device_watcher_object performSelector:input_device_watcher_selector withObject:cur_input_device_uid];
        }
        else {
            NSLog(@"Output device changed to %@", [combo objectValueOfSelectedItem]);
            cur_output_device_uid = [audio_output_device_dict allKeysForObject:[combo objectValueOfSelectedItem]][0];
            [output_device_watcher_object performSelector:output_device_watcher_selector withObject:cur_output_device_uid];
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

-(NSString*) getInputDeviceUID {
    return cur_input_device_uid;
}

-(NSString*) getOutputDeviceUID {
    return cur_output_device_uid;
}

bool DeviceHasBuffersInScope(AudioObjectID deviceID, AudioObjectPropertyScope scope) {
    if(deviceID == 0) {
        return NO;
    }

    AudioObjectPropertyAddress propertyAddress = {
        .mSelector  = kAudioDevicePropertyStreamConfiguration,
        .mScope     = scope,
        .mElement   = kAudioObjectPropertyElementWildcard
    };

    UInt32 dataSize = 0;
    OSStatus result = AudioObjectGetPropertyDataSize(deviceID, &propertyAddress, 0, NULL, &dataSize);
    if(result != kAudioHardwareNoError) {
        return NO;
    }

    AudioBufferList *bufferList = malloc(dataSize);
    if(!bufferList) {
        return NO;
    }

    result = AudioObjectGetPropertyData(deviceID, &propertyAddress, 0, NULL, &dataSize, bufferList);
    if(result != kAudioHardwareNoError) {
        free(bufferList);
        return NO;
    }

    BOOL supportsScope = bufferList->mNumberBuffers > 0;
    free(bufferList);

    return supportsScope;
}

-(void) scanDevices {
    
    self->can_select = FALSE;
    
    NSLog(@"Scanning audio devices...");
    
    [self.inputDeviceComboBox removeAllItems];
    [self.outputDeviceComboBox removeAllItems];
    
    AudioDeviceID devices[12];
    UInt32 arraySize = sizeof(devices);

    AudioObjectPropertyAddress thePropertyAddress = { kAudioHardwarePropertyDevices,
                                                    kAudioObjectPropertyScopeGlobal,
                                                    kAudioObjectPropertyElementMain };

    AudioObjectGetPropertyData(kAudioObjectSystemObject,
                               &thePropertyAddress,
                               0,
                               NULL,
                               &arraySize,
                               &devices);
    
    for(uint32_t i = 0; i < 12; i++) {
        OSStatus theErr;
        UInt32 prop_size = 0;
                
        bool isOutput = DeviceHasBuffersInScope(devices[i], kAudioObjectPropertyScopeOutput);
        bool isInput = DeviceHasBuffersInScope(devices[i], kAudioObjectPropertyScopeInput);
        
        if(isOutput) {
            
            AudioObjectPropertyAddress prop_addr = { kAudioDevicePropertyDeviceUID,
                                                   kAudioObjectPropertyScopeGlobal,
                                                   kAudioObjectPropertyElementMain };
        
            theErr = AudioObjectGetPropertyDataSize(devices[i],
                                                    &prop_addr,
                                                    0,
                                                    NULL,
                                                    &prop_size);
        
            assert(theErr == noErr);
            
            CFStringRef device_uid_cfstr = nil;
            prop_size = sizeof(device_uid_cfstr);
        
            theErr = AudioObjectGetPropertyData(devices[i],
                                       &prop_addr,
                                       0,
                                       NULL,
                                       &prop_size,
                                       (void*)&device_uid_cfstr);
            
            NSLog(@"OUTPUT DEVICE UID: %@", (__bridge NSString*) device_uid_cfstr);
            
            prop_addr.mSelector = kAudioObjectPropertyName;
            
            CFStringRef device_name_cfstr = nil;
            theErr = AudioObjectGetPropertyData(devices[i],
                                       &prop_addr,
                                       0,
                                       NULL,
                                       &prop_size,
                                       (void*)&device_name_cfstr);
            
            NSLog(@"OUTPUT DEVICE NAME: %@", (__bridge NSString*) device_name_cfstr);
            
            [audio_output_device_dict setValue:(__bridge NSString*)device_name_cfstr forKey:(__bridge NSString*)device_uid_cfstr];
            
            [self.outputDeviceComboBox addItemWithObjectValue:(__bridge NSString*)device_name_cfstr];
        }
        if(isInput) {
            
            AudioObjectPropertyAddress prop_addr = { kAudioDevicePropertyDeviceUID,
                                                   kAudioObjectPropertyScopeGlobal,
                                                   kAudioObjectPropertyElementMain };
        
            theErr = AudioObjectGetPropertyDataSize(devices[i],
                                                    &prop_addr,
                                                    0,
                                                    NULL,
                                                    &prop_size);
        
            assert(theErr == noErr);
            
            CFStringRef device_uid_cfstr = nil;
            prop_size = sizeof(device_uid_cfstr);
        
            theErr = AudioObjectGetPropertyData(devices[i],
                                       &prop_addr,
                                       0,
                                       NULL,
                                       &prop_size,
                                       (void*)&device_uid_cfstr);
            
            NSLog(@"INPUT DEVICE UID: %@", (__bridge NSString*) device_uid_cfstr);
            
            prop_addr.mSelector = kAudioObjectPropertyName;
            CFStringRef device_name_cfstr = nil;
            
            theErr = AudioObjectGetPropertyData(devices[i],
                                       &prop_addr,
                                       0,
                                       NULL,
                                       &prop_size,
                                       (void*)&device_name_cfstr);
            
            NSLog(@"INPUT DEVICE NAME: %@", (__bridge NSString*) device_name_cfstr);
            
            [audio_input_device_dict setValue:(__bridge NSString*)device_name_cfstr forKey:(__bridge NSString*)device_uid_cfstr];
            
            [self.inputDeviceComboBox addItemWithObjectValue:(__bridge NSString*)device_name_cfstr];
        }
        
        NSLog(@"DeviceID %d input? %d output? %d", devices[i], isInput, isOutput);
        
    }
    
    NSLog(@"Finished scanning audio devices.");
    
    [self.outputDeviceComboBox selectItemWithObjectValue:[audio_output_device_dict objectForKey:cur_output_device_uid]];
    [self.inputDeviceComboBox selectItemWithObjectValue:[audio_input_device_dict objectForKey:cur_input_device_uid]];
    
    self->can_select = TRUE;
}
@end
