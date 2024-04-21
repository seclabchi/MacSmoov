//
//  Patchbay.h
//  MacSmoov
//
//  Created by Paul Zaremba on 3/23/24.
//

#import <Cocoa/Cocoa.h>
#import <AudioToolbox/AudioToolbox.h>

NS_ASSUME_NONNULL_BEGIN

@interface Patchbay : NSViewController {
    id delegate;
}

- (id)init;
- (void)showPanel;


@end

NS_ASSUME_NONNULL_END
