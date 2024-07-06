//
//  MultibandObjectController.m
//  MacSmoov
//
//  Created by Paul Zaremba on 7/6/24.
//

#import <Foundation/Foundation.h>
#import "MultibandObjectController.h"

@interface MultibandObjectController () {
}

@end

@implementation MultibandObjectController

-(id) init {
    self = [super init];
    
    if(self) {
        _b1_dict = [[NSMutableDictionary alloc] init];
        _b2_dict = [[NSMutableDictionary alloc] init];
        _b3_dict = [[NSMutableDictionary alloc] init];
        _b4_dict = [[NSMutableDictionary alloc] init];
        _b5_dict = [[NSMutableDictionary alloc] init];
        _the_dict = _b1_dict;
    }
    
    return self;
}

-(void) selectBand:(uint8_t) band {
    [self willChangeValueForKey:@"the_dict"];
    
    switch(band) {
        case 1:
            _the_dict = _b1_dict;
            break;
        case 2:
            _the_dict = _b2_dict;
            break;
        case 3:
            _the_dict = _b3_dict;
            break;
        case 4:
            _the_dict = _b4_dict;
            break;
        case 5:
            _the_dict = _b5_dict;
            break;
        default:
            NSLog(@"Bad band %d selected", band);
    }
    
    [self didChangeValueForKey:@"the_dict"];
}

-(void) dumpSettings {
    NSLog(@"MULTIBAND SETTINGS DUMP");
    NSLog(@"BAND 1: %@", _b1_dict);
    NSLog(@"BAND 2: %@", _b2_dict);
    NSLog(@"BAND 3: %@", _b3_dict);
    NSLog(@"BAND 4: %@", _b4_dict);
    NSLog(@"BAND 5: %@", _b5_dict);
}

@end
