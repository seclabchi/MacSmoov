//
//  CompressionMeter.m
//  MacSmoov
//
//  Created by Paul Zaremba on 8/22/23.
//

#import "CompressionMeter.h"


@interface CompressionMeter() {
    Float32 min_lev;
    Float32 max_lev;
    NSColor* meter_color;
}
@end

@implementation CompressionMeter

- (void) awakeFromNib {
    self->min_lev = 0.0;
    self->max_lev = -25.0;
    comp_lo = -5.0;
    comp_hi = -15.0;
}

- (id)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        self->min_lev = 0.0;
        self->max_lev = -30.0;
        comp_lo = min_lev/2.0;
        comp_hi = min_lev/2.0;
    }
    return self;
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    // Drawing code here.
    // erase the background by drawing lightgray
    [[NSColor darkGrayColor] set];
    [NSBezierPath fillRect:dirtyRect];
    NSRect rect_lo;
    rect_lo.origin.x = 0.0;
    //rect_lo.origin.y = dirtyRect.size.height;
    rect_lo.size.width = dirtyRect.size.width/2.0 - 1.0;
    
    NSRect rect_hi;
    rect_hi.origin.x = dirtyRect.size.width/2.0 + 1.0;
    //rect_hi.origin.y = dirtyRect.size.height;
    rect_hi.size.width = dirtyRect.size.width/2.0;
    
    //max_lev - min_lev is total logical range
    //dirtyRect.size.height is total physical range
    
    Float32 scaleFactor_lo = ((comp_lo)/(max_lev));
    Float32 scaleFactor_hi = ((comp_hi)/(max_lev));
    
    rect_lo.origin.y = dirtyRect.size.height - (dirtyRect.size.height * scaleFactor_lo);
    rect_hi.origin.y = dirtyRect.size.height - (dirtyRect.size.height * scaleFactor_hi);
    
    rect_lo.size.height = dirtyRect.size.height - rect_lo.origin.y;
    rect_hi.size.height = dirtyRect.size.height - rect_hi.origin.y;
    
    
    [meter_color set];
    [NSBezierPath fillRect:rect_lo];
    [NSBezierPath fillRect:rect_hi];
}

- (void)set_comp_lo:(Float32)locomp hi:(Float32)hicomp {
    comp_lo = locomp;
    comp_hi = hicomp;
    self.needsDisplay = true;
}

-(void) set_meter_color:(NSColor*)_color {
    meter_color = _color;
}

- (void)set_meter_range:(float)_max_range {
    max_lev = _max_range;
}

@end
