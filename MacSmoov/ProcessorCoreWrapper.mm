//
//  ProcessorCoreWrapper.m
//  MacSmoov
//
//  Created by Paul Zaremba on 4/28/23.
//

#import <Foundation/Foundation.h>
#import "ProcessorCoreWrapper.h"
#import "ProcessorCore.hpp"

@interface ProcessorCoreWrapper ()
@property (nonatomic, readwrite, assign) fmsmoov::ProcessorCore *cpp;
@end

@implementation ProcessorCoreWrapper
@synthesize cpp = _cpp;

-(id) initWithSampleRate:(uint32_t)sample_rate numberOfChannels:(uint32_t)num_chans bufSize:(uint32_t)buf_size {
    self = [super init];
    if(self) {
        _cpp = new fmsmoov::ProcessorCore(sample_rate, num_chans, buf_size);
    }
    
    return self;
}

-(void) processWithInput:(float*)in_buf output:(float*)out_buf ofSize:(uint32_t) n_frames {
    _cpp->process(in_buf, out_buf, n_frames);
}

@end
