//
//  Patchbay.m
//  MacSmoov
//
//  Created by Paul Zaremba on 4/8/23.
//

#import "Patchbay.h"

@interface Patchbay ()

@end

@implementation Patchbay {
    
}

- (id)init {
    
    NSLog(@"Initing patchbay...");
    
    self = [super init];
    
    if(self) {
        
    }
    
    NSLog(@"patchbay panel inited.");
    
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do view setup here.
}

-(void)showPanel {
    NSPanel* panel = (NSPanel*)[[self view] window];
    //[_audio_device_selector_panel setFloatingPanel:YES];
    //[_audio_device_selector_panel orderFront:nil];
    //_input_device_combo.delegate = self;
    //_output_device_combo.delegate = self;
}



@end
