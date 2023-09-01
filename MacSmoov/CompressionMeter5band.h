//
//  CompressionMeter5band.h
//  MacSmoov
//
//  Created by Paul Zaremba on 8/26/23.
//

#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN

@interface CompressionMeter5band : NSControl {
    Float32 comp_b1;
    Float32 comp_b2;
    Float32 comp_b3;
    Float32 comp_b4;
    Float32 comp_b5;
}

- (void)set_meter_color:(NSColor*)color;
- (void)set_meter_range:(float)_max_range;
- (void)set_comps:(float**)comps;

@end

NS_ASSUME_NONNULL_END
