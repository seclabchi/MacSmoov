//
//  AudioDevice.h
//  MacSmoov
//
//  Created by Paul Zaremba on 8/8/23.
//

#ifndef AudioDevice_h
#define AudioDevice_h

#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>

@interface AudioDevice : NSObject {
    AudioDeviceID device_id;
    CFStringRef device_uid;
    CFStringRef device_name;
}

@property AudioDeviceID device_id;
@property CFStringRef device_uid;
@property CFStringRef device_name;

@end

#endif /* AudioDevice_h */
