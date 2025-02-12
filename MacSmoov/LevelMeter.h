//
//  LevelMeter.h
//  MacSmoov
//
//  Created by Paul Zaremba on 8/6/23.
//

#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN

@interface LevelMeter : NSControl {
    Float32 level_l_rms;
    Float32 level_r_rms;
    Float32 level_l_peak;
    Float32 level_r_peak;
}

- (void)awakeFromNib;
- (void)setMeterDbMinVal:(Float32)minVal maxVal:(Float32)maxVal;
- (void)setMeterLinearMinVal:(Float32)minVal maxVal:(Float32)maxVal;
- (void)set_levels_Lrms:(Float32)lrms Rrms:(Float32)rrms Lpeak:(Float32)lpeak Rpeak:(Float32)rpeak;
- (void)set_level:(Float32)level;

@end

NS_ASSUME_NONNULL_END
