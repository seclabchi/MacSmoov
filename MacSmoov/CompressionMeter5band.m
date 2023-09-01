//
//  CompressionMeter5band.m
//  MacSmoov
//
//  Created by Paul Zaremba on 8/22/23.
//

#import "CompressionMeter5band.h"


@interface CompressionMeter5band() {
    Float32 min_lev;
    Float32 max_lev;
    NSColor* meter_color;
}
@end

@implementation CompressionMeter5band

- (void) awakeFromNib {
    self->min_lev = 0.0;
    self->max_lev = -25.0;
    comp_b1 = -3.0;
    comp_b2 = -6.0;
    comp_b3 = -9.0;
    comp_b4 = -12.0;
    comp_b5 = -15.0;
}

- (id)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        self->min_lev = 0.0;
        self->max_lev = -25.0;
        comp_b1 = -3.0;
        comp_b2 = -6.0;
        comp_b3 = -9.0;
        comp_b4 = -12.0;
        comp_b5 = -15.0;
    }
    return self;
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    // Drawing code here.
    // erase the background by drawing lightgray
    [[NSColor blackColor] set];
    [NSBezierPath fillRect:dirtyRect];
    NSRect rect_b1;
    rect_b1.origin.x = 0.0;
    rect_b1.size.width = dirtyRect.size.width/5.0 - 1.0;
    
    NSRect rect_b2;
    rect_b2.origin.x = dirtyRect.size.width/5.0;
    rect_b2.size.width = dirtyRect.size.width/5.0 - 1.0;
    
    NSRect rect_b3;
    rect_b3.origin.x = 2 * dirtyRect.size.width/5.0;
    rect_b3.size.width = dirtyRect.size.width/5.0 - 1.0;
    
    NSRect rect_b4;
    rect_b4.origin.x = 3 * dirtyRect.size.width/5.0;
    rect_b4.size.width = dirtyRect.size.width/5.0 - 1.0;
    
    NSRect rect_b5;
    rect_b5.origin.x = 4 * dirtyRect.size.width/5.0;
    rect_b5.size.width = dirtyRect.size.width/5.0 - 1.0;
    
    //max_lev - min_lev is total logical range
    //dirtyRect.size.height is total physical range
    
    Float32 scaleFactor_b1 = ((comp_b1)/(max_lev));
    Float32 scaleFactor_b2 = ((comp_b2)/(max_lev));
    Float32 scaleFactor_b3 = ((comp_b3)/(max_lev));
    Float32 scaleFactor_b4 = ((comp_b4)/(max_lev));
    Float32 scaleFactor_b5 = ((comp_b5)/(max_lev));
    
    rect_b1.origin.y = dirtyRect.size.height - (dirtyRect.size.height * scaleFactor_b1);
    rect_b2.origin.y = dirtyRect.size.height - (dirtyRect.size.height * scaleFactor_b2);
    rect_b3.origin.y = dirtyRect.size.height - (dirtyRect.size.height * scaleFactor_b3);
    rect_b4.origin.y = dirtyRect.size.height - (dirtyRect.size.height * scaleFactor_b4);
    rect_b5.origin.y = dirtyRect.size.height - (dirtyRect.size.height * scaleFactor_b5);
    
    rect_b1.size.height = dirtyRect.size.height - rect_b1.origin.y;
    rect_b2.size.height = dirtyRect.size.height - rect_b2.origin.y;
    rect_b3.size.height = dirtyRect.size.height - rect_b3.origin.y;
    rect_b4.size.height = dirtyRect.size.height - rect_b4.origin.y;
    rect_b5.size.height = dirtyRect.size.height - rect_b5.origin.y;
    
    [meter_color set];
    [NSBezierPath fillRect:rect_b1];
    [NSBezierPath fillRect:rect_b2];
    [NSBezierPath fillRect:rect_b3];
    [NSBezierPath fillRect:rect_b4];
    [NSBezierPath fillRect:rect_b5];
}

- (void)set_comps:(float**)comps {
    comp_b1 = *comps[0];
    comp_b2 = *comps[1];
    comp_b3 = *comps[2];
    comp_b4 = *comps[3];
    comp_b5 = *comps[4];
    self.needsDisplay = true;
}

-(void) set_meter_color:(NSColor*)_color {
    meter_color = _color;
}

- (void)set_meter_range:(float)_max_range {
    max_lev = _max_range;
}

@end
