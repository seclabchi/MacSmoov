//
//  ProcessorSysInterface.m
//  MacSmoov
//
//  Created by Paul Zaremba on 4/16/23.
//

#import "ProcessorSysInterface.h"

@interface ProcessorSysInterface() {
    NSUserDefaults *prefs;
}
@end

@implementation ProcessorSysInterface

float* input_buffer;
float* output_buffer;
uint32_t interleaved_buf_len_in_bytes;
uint32_t interleaved_buf_len_in_floats;

/* The preliminary work I'm doing with the AUHAL is giving me non-interleaved stereo samples, so we're just gonna
   run with that for the time being.
 */
-(id) initWithSampleRate:(NSUInteger)sample_rate numberOfChannels:(UInt8)channels bufferSize:(NSUInteger)bufSize {
    self = [super init];
    if(self) {
        NSUserDefaultsController *prefs_controller = [NSUserDefaultsController sharedUserDefaultsController];
        prefs = prefs_controller.defaults;
        interleaved_buf_len_in_bytes = bufSize * channels;
        interleaved_buf_len_in_floats = interleaved_buf_len_in_bytes / sizeof(float);
        core = [[ProcessorCoreWrapper alloc] initWithSampleRate:sample_rate numberOfChannels:channels bufSize:interleaved_buf_len_in_bytes];
        
        input_buffer = (float*)malloc(interleaved_buf_len_in_bytes);
        output_buffer = (float*)malloc(interleaved_buf_len_in_bytes);
    }
    
    return self;
}

-(void) processWithInput:(AudioBufferList*)buf_list {
    //we're dealing with non-interleaved buffers here, so we're going to interleave them before passing them to the core.
    //Is this a shitty idea?  Is it better to work with interleaved vs. non-interleaved buffers?  Whatever solution we come
    //up with needs to be platform-agnostic in the core.  The work done to date with FMSMoov on the Linux platform uses
    //interleaved samples, so we'll do the buffer conversion here.  Not sure at this point what the best course of action
    //is.
    uint32_t j = 0;
    float* inbuf00 = (float*)(buf_list->mBuffers[0].mData);
    float* inbuf01 = (float*)(buf_list->mBuffers[1].mData);
    uint32_t count = (buf_list->mBuffers[0].mDataByteSize)/sizeof(float);
    
    for(uint32_t i = 0; i < count; i++) {
        input_buffer[j] = inbuf00[i];
        input_buffer[j+1] = inbuf01[i];
        j+=2;
    }
    
    //we should now have an interleaved buffer of input samples.
    //memcpy(output_buffer, input_buffer, count * 2 * sizeof(float));
    [core processWithInput:input_buffer output:output_buffer ofSize:interleaved_buf_len_in_floats];
    
    //now need to de-interleave the samples to feed back to the AUHAL
    j = 0;
    
    for(uint32_t i = 0; i < count; i++) {
        inbuf00[i] = output_buffer[j];
        inbuf01[i] = output_buffer[j+1];
        j+=2;
    }
    
    
}

-(void) getMainInLevelsLrms:(float*)lrms Rrms:(float*)rrms Lpeak:(float*)lpeak Rpeak:(float*)rpeak {
    [core getMainInLevelsLrms:lrms Rrms:rrms Lpeak:lpeak Rpeak:rpeak];
}

-(void) setMainInGainDBL:(float)mainInL R:(float)mainInR {
    [core setMainInGainDBL:mainInL R:mainInR];
}

@end
