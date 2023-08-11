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
          
    }
    return self;
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    NSLog(@"Redrawing level meter");
    
    // Drawing code here.
    // erase the background by drawing white
    [[NSColor darkGrayColor] set];
    [NSBezierPath fillRect:dirtyRect];
    NSRect rect_l;
    rect_l.origin.x = 0.0;
    rect_l.origin.y = 0.0;
    rect_l.size.width = dirtyRect.size.width/2.0;
    
    NSRect rect_r;
    rect_r.origin.x = dirtyRect.size.width/2.0;
    rect_r.origin.y = 0.0;
    rect_r.size.width = dirtyRect.size.width/2.0;
    
    //max_lev - min_lev is total logical range
    //dirtyRect.size.height is total physical range
    
    Float32 scaleFactor_l = ((min_lev - level_l)/(min_lev - max_lev)) * dirtyRect.size.height;
    Float32 scaleFactor_r = ((min_lev - level_r)/(min_lev - max_lev)) * dirtyRect.size.height;
    
    rect_l.size.height = scaleFactor_l;
    rect_r.size.height = scaleFactor_r;
    
    [[NSColor yellowColor] set];
    [NSBezierPath fillRect:rect_l];
    [NSBezierPath fillRect:rect_r];
    
}

- (void)set_levels_left:(Float32)_level_l right:(Float32)_level_r {
    level_l = _level_l;
    level_r = _level_r;
    self.needsDisplay = true;
}


@end
