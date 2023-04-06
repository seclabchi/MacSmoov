//
//  SignalGeneratorWrapper.m
//  MacSmoov
//
//  Created by Paul Zaremba on 4/2/23.
//

#import <Foundation/Foundation.h>
#import "SignalGeneratorWrapper.h"
#import "SignalGenerator.hpp"

@interface SignalGenerator ()
@property (nonatomic, readwrite, assign) fmsmoov::SignalGenerator *cpp;
@end

@implementation SignalGenerator
@synthesize cpp = _cpp;

-(id) initWithSampleRate:(uint32_t)sample_rate numberOfChannels:(uint32_t)num_chans {
    self = [super init];
    if(self) {
        _cpp = new fmsmoov::SignalGenerator(sample_rate, num_chans);
    }
    
    return self;
}

-(void) configureWithType:(SIG_GEN_TYPE) type frequency:(uint32_t)frequency {
    _cpp->configure(type, frequency);
}

-(void) getNextBuffer:(float*) out_buf ofSize:(uint32_t) n_frames {
    _cpp->get_next_buffers(out_buf, n_frames);
}

-(void) adjustVolume:(float) volume {
    _cpp->adjust_volume(volume);
}
@end
