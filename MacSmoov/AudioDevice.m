//
//  AudioDevice.m
//  MacSmoov
//
//  Created by Paul Zaremba on 8/8/23.
//

#import "AudioDevice.h"

@implementation AudioDevice

@synthesize device_id;
@synthesize device_uid;
@synthesize device_name;

-(void) print_device {
    NSLog(@"%d: %@ (%@)", device_id, device_name, device_uid);
}

@end
