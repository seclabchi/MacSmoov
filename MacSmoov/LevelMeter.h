//
//  LevelMeter.h
//  MacSmoov
//
//  Created by Paul Zaremba on 8/6/23.
//

#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN

@interface LevelMeter : NSControl {
    Float32 level_l;
    Float32 level_r;
}

- (void)set_levels_left:(Float32)_level_l right:(Float32)_level_r;

@end

NS_ASSUME_NONNULL_END
