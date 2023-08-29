//
//  MultibandControlsView.m
//  MacSmoov
//
//  Created by Paul Zaremba on 8/28/23.
//

#import "MultibandControlsView.h"

@interface MultibandControlsView ()

@end

@implementation MultibandControlsView

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do view setup here.
    NSLog(@"MultibandControlsView viewDidLoad");
}

-(IBAction) band_select:(NSButton*)sender {
    NSLog(@"Band %lu selected.", (long)[sender tag]);
}

-(void)showPanel {
    NSPanel* panel = (NSPanel*)[[self view] window];
}


@end
