//
//  CoreConfig.h
//  MacSmoov
//
//  Created by Paul Zaremba on 4/13/24.
//

#ifndef CoreConfig_h
#define CoreConfig_h

#include "core_common.h"

namespace fmsmoov {
class CoreConfig {
public:
    static CoreConfig* get_instance();
    MULTIBAND_PARAMS get_mb_params();
private:
    CoreConfig();
    MULTIBAND_PARAMS multiband_params;
};

}

#endif /* CoreConfig_h */
