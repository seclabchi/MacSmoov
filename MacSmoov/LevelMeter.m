//
//  LevelMeter.m
//  MacSmoov
//
//  Created by Paul Zaremba on 8/6/23.
//

#import "LevelMeter.h"

@interface LevelMeter() {
    Float32 max_lev;
    Float32 min_lev;
    bool is_linear;
    NSColor* color_rms;
    NSColor* color_peak;
}
@end

@implementation LevelMeter

- (void) awakeFromNib {
    level_l_rms = min_lev;
    level_r_rms = min_lev;
    level_l_peak = min_lev;
    level_r_peak = min_lev;
    self->max_lev = 0.0;
    self->min_lev = -30.0;
    self->is_linear = false;
    self->color_rms = [NSColor cyanColor];
    self->color_peak = [NSColor colorWithRed:1.0 green:0.7 blue:0.7 alpha:1.0];
}

- (id)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        level_l_rms = min_lev;
        level_r_rms = min_lev;
        level_l_peak = min_lev;
        level_r_peak = min_lev;
        self->max_lev = 0.0;
        self->min_lev = -30.0;
        self->is_linear = false;
    }
    return self;
}

- (void)setMeterDbMinVal:(Float32)minVal maxVal:(Float32)maxVal {
    self->is_linear = false;
    self->min_lev = minVal;
    self->max_lev = maxVal;
}

- (void)setMeterLinearMinVal:(Float32)minVal maxVal:(Float32)maxVal {
    self->is_linear = true;
    self->min_lev = minVal;
    self->max_lev = maxVal;
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    //NSLog(@"Redrawing level meter");
    
    // Drawing code here.
    // erase the background by drawing white
    [[NSColor grayColor] set];
    [NSBezierPath fillRect:dirtyRect];
    NSRect rect_l_rms;
    rect_l_rms.origin.x = 0.0;
    rect_l_rms.origin.y = 0.0;
    rect_l_rms.size.width = dirtyRect.size.width/2.0 - 1.0;
    NSRect rect_l_peak = rect_l_rms;
    
    NSRect rect_r_rms;
    rect_r_rms.origin.x = dirtyRect.size.width/2.0 + 1.0;
    rect_r_rms.origin.y = 0.0;
    rect_r_rms.size.width = dirtyRect.size.width/2.0;
    NSRect rect_r_peak = rect_r_rms;
    
    //max_lev - min_lev is total logical range
    //dirtyRect.size.height is total physical range
    
    Float32 scaleFactor_l_rms = ((min_lev - level_l_rms)/(min_lev - max_lev)) * dirtyRect.size.height;
    Float32 scaleFactor_r_rms = ((min_lev - level_r_rms)/(min_lev - max_lev)) * dirtyRect.size.height;
    Float32 scaleFactor_l_peak = ((min_lev - level_l_peak)/(min_lev - max_lev)) * dirtyRect.size.height;
    Float32 scaleFactor_r_peak = ((min_lev - level_r_peak)/(min_lev - max_lev)) * dirtyRect.size.height;
    
    rect_l_rms.size.height = scaleFactor_l_rms;
    rect_r_rms.size.height = scaleFactor_r_rms;
    rect_l_peak.size.height = scaleFactor_l_peak;
    rect_r_peak.size.height = scaleFactor_r_peak;
    
    [self->color_peak set];
    [NSBezierPath fillRect:rect_l_peak];
    [NSBezierPath fillRect:rect_r_peak];
    
    [self->color_rms set];
    [NSBezierPath fillRect:rect_l_rms];
    [NSBezierPath fillRect:rect_r_rms];
    
}

- (void)set_levels_Lrms:(Float32)lrms Rrms:(Float32)rrms Lpeak:(Float32)lpeak Rpeak:(Float32)rpeak {
    level_l_rms = lrms;
    level_r_rms = rrms;
    level_l_peak = lpeak;
    level_r_peak = rpeak;
    self.needsDisplay = true;
}

- (void)set_level:(Float32)level {
    level_l_rms = 0.0;
    level_r_rms = 0.0;
    level_l_peak = level;
    level_r_peak = level;
    self.needsDisplay = true;
}

- (void)set_levelsL:(Float32)level_l R:(Float32)level_r {
    level_l_rms = 0.0;
    level_r_rms = 0.0;
    level_l_peak = level_l;
    level_r_peak = level_r;
    self.needsDisplay = true;
}

- (void)set_meter_colors_rms:(NSColor* _Nonnull)colorRms peak:(NSColor* _Nonnull)colorPeak {
    self->color_rms = colorRms;
    self->color_peak = colorPeak;
}



@end
