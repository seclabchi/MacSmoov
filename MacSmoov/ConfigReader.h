//
//  ConfigReader.h
//  MacSmoov
//
//  Created by Paul Zaremba on 3/24/24.
//

#ifndef ConfigReader_h
#define ConfigReader_h

#include "core/core_common.h"

@interface ConfigReader : NSObject {
    
}

-(id) init;
-(BOOL) readCfgFromFile:(NSString*)filename;
-(BOOL) writeCfgToFile:(NSString*)filename;

-(AGC_PARAMS) getAgcParams;


@end

#endif /* ConfigReader_h */
