//
//  SignalGeneratorViewController.m
//  MacSmoov
//
//  Created by Paul Zaremba on 4/2/23.
//

#import "SignalGeneratorViewController.h"

@interface SignalGeneratorViewController ()
@property (strong) IBOutlet NSPanel *sig_gen_panel;
@end

@implementation SignalGeneratorViewController

-(IBAction) showSignalGenerators:(id)sender {
    [_sig_gen_panel setFloatingPanel:YES];
    [_sig_gen_panel orderFront:nil];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do view setup here.
}

@end
