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
    void get_input_gain(std::pair<float, float>& gain);
    void set_input_gain(const std::pair<float, float>& gain);
    bool get_input_level_enabled();
    void set_input_level_enabled(bool enabled);
    void get_agc_params(AGC_PARAMS& params);
    void set_agc_params(const AGC_PARAMS& params);
    bool get_hf_enhance_enabled();
    void set_hf_enhance_enabled(bool enabled);
    bool get_mb_crossover_enabled();
    void set_mb_crossover_enabled(bool enabled);
    bool get_mb_compressor_enabled();
    void set_mb_compressor_enabled(bool enabled);
    bool get_mb_limiter_enabled();
    void set_mb_limiter_enabled(bool enabled);
    void get_mb_params(MULTIBAND_PARAMS& params);
    void set_mb_params(const MULTIBAND_PARAMS& params);
    bool load_cfg_from_file(const std::string& filename);
    bool write_cfg_to_file(const std::string& filename);
private:
    CoreConfig();
    YAML::Node yaml_node;
    std::string version;
    std::string cfg_name;
    bool enable_input_gain;
    bool enable_input_level;
    bool enable_2band_agc;
    bool enable_hf_enhance;
    bool enable_mb_crossover;
    bool enable_mb_compressor;
    bool enable_mb_limiter;
    std::pair<float, float> input_gain;
    AGC_PARAMS agc_params;
    MULTIBAND_PARAMS multiband_params;
};

}

#endif /* CoreConfig_h */
