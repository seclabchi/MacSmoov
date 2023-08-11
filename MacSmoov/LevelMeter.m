//
//  LevelMeter.m
//  MacSmoov
//
//  Created by Paul Zaremba on 8/6/23.
//

#import "LevelMeter.h"

@implementation LevelMeter

Float32 max_lev = 0.0;
Float32 min_lev = -96.0;

- (id)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        level_l_rms = -100.0;
        level_r_rms = -100.0;
        level_l_peak = -100.0;
        level_r_peak = -100.0;
    }
    return self;
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    //NSLog(@"Redrawing level meter");
    
    // Drawing code here.
    // erase the background by drawing white
    [[NSColor darkGrayColor] set];
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
    
    [[NSColor yellowColor] set];
    [NSBezierPath fillRect:rect_l_peak];
    [NSBezierPath fillRect:rect_r_peak];
    
    [[NSColor greenColor] set];
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


@end
