//
//  CompressionMeter.h
//  MacSmoov
//
//  Created by Paul Zaremba on 8/22/23.
//

#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN

@interface CompressionMeter : NSControl {
    Float32 comp_lo;
    Float32 comp_hi;
}

- (void)set_meter_color:(NSColor*)color;
- (void)set_meter_range:(float)_max_range;
- (void)set_comp_lo:(Float32)locomp hi:(Float32)hicomp;

@end

NS_ASSUME_NONNULL_END
