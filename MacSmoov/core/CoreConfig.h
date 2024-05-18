//
//  CoreConfig.h
//  MacSmoov
//
//  Created by Paul Zaremba on 4/13/24.
//

#ifndef CoreConfig_h
#define CoreConfig_h

#include <iostream>
#include <yaml-cpp/yaml.h>
#include "core_common.h"

namespace fmsmoov {

class CoreConfig {
public:
    static CoreConfig* get_instance();
    void get_agc_params(AGC_PARAMS& params);
    void get_mb_params(MULTIBAND_PARAMS& params);
    bool load_cfg_from_file(const std::string& filename);
private:
    CoreConfig();
    YAML::Node yaml_node;
    std::string version;
    std::string cfg_name;
    std::pair<double, double> input_gain;
    AGC_PARAMS agc_params;
    MULTIBAND_PARAMS multiband_params;
};

}

#endif /* CoreConfig_h */
