//
//  CoreConfig.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 4/13/24.
//

#include <stdio.h>
#include <iostream>
#include <fstream>
#include "CoreConfig.h"

namespace fmsmoov {

static CoreConfig* the_instance = nullptr;

/*
 * ===================================================================================
 * DEFAULT FALLBACK FACTORY SETTINGS.  These are loaded if a config file is not found.
 * ===================================================================================
 */

/*===================================*/
/* 2-BAND AGC                        */
/*===================================*/
static const AGC_PARAMS agc_params_default = {
    .drive = -42.0,
    .release_master = 0.300,
    .release_bass = 0.600,
    .gate_thresh = -24.0,
    .bass_coupling = 0.3,
    .window_size = -3.0,
    .window_release = 60,
    .ratio = 1000.0,
    .bass_thresh = 0.0,
    .idle_gain = 0.0,
    .attack_master = 8.000,
    .attack_bass = 8.000,
    .post_gain = 22.0,
};

/*===================================*/
/* MULTIBAND COMPRESSORS             */
/*===================================*/

static const COMPRESSOR_PARAMS comp_params_b1_default = {
     .drive = -40.0,
     .release = .250,
     .gate_thresh = -50.0,
     .use_coupling = false,
     .coupling = 0.0,
     .window_size = 0.0,
     .window_release = 2.0,
     .ratio = 2.5,
     .idle_gain = 0.0,
     .attack = 0.125,
     .post_gain = 0.0
 };
 
static const COMPRESSOR_PARAMS comp_params_b2_default = {
     .drive = -40.0,
     .release = 0.240,
     .gate_thresh = -50.0,
     .use_coupling = false,
     .coupling = 0.0,
     .window_size = 0.0,
     .window_release = 2.0,
     .ratio = 1.7,
     .idle_gain = 0.0,
     .attack = 0.080,
     .post_gain = 0.0
 };
 
static const COMPRESSOR_PARAMS comp_params_b3_default = {
     .drive = -40.0,
     .release = .048,
     .gate_thresh = -50.0,
     .use_coupling = false,
     .coupling = 0.0,
     .window_size = 0.0,
     .window_release = 2.0,
     .ratio = 2,
     .idle_gain = 0.0,
     .attack = 0.016,
     .post_gain = 0.0
 };
 
static const COMPRESSOR_PARAMS comp_params_b4_default = {
     .drive = -40.0,
     .release = 0.032,
     .gate_thresh = -50.0,
     .use_coupling = true,
     .coupling = 0.20,
     .window_size = 0.0,
     .window_release = 2.0,
     .ratio = 2.2,
     .idle_gain = 0.0,
     .attack = 0.008,
     .post_gain = 0.0
 };
 
static const COMPRESSOR_PARAMS comp_params_b5_default = {
     .drive = -40.0,
     .release = .016,
     .gate_thresh = -50.0,
     .use_coupling = true,
     .coupling = 0.20,
     .window_size = 0.0,
     .window_release = 2.0,
     .ratio = 2.4,
     .idle_gain = 0.0,
     .attack = 0.004,
     .post_gain = 0.0
 };
 
 /*===================================*/
 /* LIMITERS                          */
 /*===================================*/


static const COMPRESSOR_PARAMS lim_params_b1_default = {
     .drive = -49.0,
     .release = 0.125,
     .gate_thresh = -100.0,
     .use_coupling = false,
     .coupling = 0.0,
     .window_size = 0.0,
     .window_release = 0.0,
     .ratio = 9,
     .idle_gain = 0.0,
     .attack = 0.100,
     .post_gain = 14.0
 };
 
static const COMPRESSOR_PARAMS lim_params_b2_default = {
     .drive = -49.0,
     .release = 0.080,
     .gate_thresh = -100.0,
     .use_coupling = false,
     .coupling = 0.0,
     .window_size = 0.0,
     .window_release = 0.0,
     .ratio = 9,
     .idle_gain = 0.0,
     .attack = 0.080,
     .post_gain = 8.0
 };
 
static const COMPRESSOR_PARAMS lim_params_b3_default = {
     .drive = -49.0,
     .release = 0.016,
     .gate_thresh = -100.0,
     .use_coupling = false,
     .coupling = 0.0,
     .window_size = 0.0,
     .window_release = 0.0,
     .ratio = 9,
     .idle_gain = 0.0,
     .attack = 0.016,
     .post_gain = 8.0
 };
 
static const COMPRESSOR_PARAMS lim_params_b4_default = {
     .drive = -49.0,
     .release = 0.008,
     .gate_thresh = -100.0,
     .use_coupling = false,
     .coupling = 0.0,
     .window_size = 0.0,
     .window_release = 0.0,
     .ratio = 9,
     .idle_gain = 0.0,
     .attack = 0.008,
     .post_gain = 8.0
 };
 
static const COMPRESSOR_PARAMS lim_params_b5_default = {
     .drive = -49.0,
     .release = 0.004,
     .gate_thresh = -100.0,
     .use_coupling = false,
     .coupling = 0.0,
     .window_size = 0.0,
     .window_release = 0.0,
     .ratio = 9,
     .idle_gain = 0.0,
     .attack = 0.004,
     .post_gain = 12.0
 };
 
 
static const MULTIBAND_PARAMS multiband_params_default = {
     .comp_params[0] = comp_params_b1_default,
     .lim_params[0] = lim_params_b1_default,
     .comp_params[1] = comp_params_b2_default,
     .lim_params[1] = lim_params_b2_default,
     .comp_params[2] = comp_params_b3_default,
     .lim_params[2] = lim_params_b3_default,
     .comp_params[3] = comp_params_b4_default,
     .lim_params[3] = lim_params_b4_default,
     .comp_params[4] = comp_params_b5_default,
     .lim_params[4] = lim_params_b5_default
 };
 

CoreConfig::CoreConfig() {
    version = "0.0";
    cfg_name = "default";
    enable_input_gain = true;
    input_gain = {0.0, 0.0};
    enable_input_level = true;
    enable_2band_agc = true;
    agc_params = agc_params_default;
    enable_hf_enhance = false;
    enable_mb_crossover = true;
    enable_mb_compressor = true;
    enable_mb_limiter = true;
    multiband_params = multiband_params_default;
}

CoreConfig* CoreConfig::get_instance() {
    if(nullptr == the_instance) {
        the_instance = new CoreConfig();
    }
    
    return the_instance;
}

void CoreConfig::get_input_gain(std::pair<float, float>& gain) {
    gain = input_gain;
}

void CoreConfig::set_input_gain(const std::pair<float, float>& gain) {
    input_gain = gain;
}

bool CoreConfig::get_input_level_enabled() {
    return enable_input_level;
}

void CoreConfig::set_input_level_enabled(bool enabled) {
    enable_input_level = enabled;
}

void CoreConfig::get_agc_params(AGC_PARAMS& params) {
    params = agc_params;
}

void CoreConfig::set_agc_params(const AGC_PARAMS& params) {
    agc_params = params;
}

bool CoreConfig::get_hf_enhance_enabled() {
    return enable_hf_enhance;
}

void CoreConfig::set_hf_enhance_enabled(bool enabled) {
    enable_hf_enhance = enabled;
}

bool CoreConfig::get_mb_crossover_enabled() {
    return enable_mb_crossover;
}

void CoreConfig::set_mb_crossover_enabled(bool enabled) {
    enable_mb_crossover = enabled;
}

bool CoreConfig::get_mb_compressor_enabled() {
    return enable_mb_compressor;
}

void CoreConfig::set_mb_compressor_enabled(bool enabled) {
    enable_mb_compressor = enabled;
}

bool CoreConfig::get_mb_limiter_enabled() {
    return enable_mb_limiter;
}

void CoreConfig::set_mb_limiter_enabled(bool enabled) {
    enable_mb_limiter = enabled;
}

void CoreConfig::get_mb_params(MULTIBAND_PARAMS& params) {
    params = multiband_params;
}

void CoreConfig::set_mb_params(const MULTIBAND_PARAMS& params) {
    multiband_params = params;;
}

/*
 * TODO: Rework the config file read
 * This entire section is hokey, we don't need to iterate through the map
 * looking for the keys.  The keys should be determinate.  Also, there's absolutely
 * no exception handling so if the config file read blows up the whole thing won't
 * work right.
 */
bool CoreConfig::load_cfg_from_file(const std::string &filename) {
    try {
        std::cout << "Reading config from " << filename << std::endl;
        
        std::ifstream cfg_file_stream;
        const char* cfg_file_path = filename.c_str();
        cfg_file_stream.open(cfg_file_path);
        if(cfg_file_stream.fail()) {
            std::cout << "Can't open config file for reasons..." << strerror(errno) << std::endl;
        }
        else {
            yaml_node = YAML::LoadFile(cfg_file_path);
            
            //std::cout << "Config YAML Dump:\n=================\n" << YAML::Dump(yaml_node).c_str() << std::endl;
            
            YAML::Node input_gain = yaml_node["input_gain"];
            enable_input_gain = input_gain["enabled"].as<bool>();
            input_gain = std::make_pair(input_gain["L"].as<float>(), input_gain["R"].as<float>());
            
            YAML::Node input_level = yaml_node["input_level"];
            enable_input_level = input_level["enabled"].as<bool>();
                        
            /* Read AGC Params */
            YAML::Node agc = yaml_node["agc_params"];
            agc_params.enabled = agc["enabled"].as<bool>();
            agc_params.mute_lo = agc["mute_lo"].as<bool>();
            agc_params.mute_hi = agc["mute_hi"].as<bool>();
            agc_params.drive = agc["drive"].as<float>();
            agc_params.release_master = agc["release_master"].as<float>();
            agc_params.release_bass = agc["release_bass"].as<float>();
            agc_params.gate_thresh = agc["gate_thresh"].as<float>();
            agc_params.bass_coupling = agc["bass_coupling"].as<float>();
            agc_params.window_size = agc["window_size"].as<float>();
            agc_params.window_release = agc["window_release"].as<float>();
            agc_params.ratio = agc["ratio"].as<float>();
            agc_params.bass_thresh = agc["bass_thresh"].as<float>();
            agc_params.idle_gain = agc["idle_gain"].as<float>();
            agc_params.attack_master = agc["attack_master"].as<float>();
            agc_params.attack_bass = agc["attack_bass"].as<float>();
            agc_params.post_gain = agc["post_gain"].as<float>();
        
            std::cout << "Loaded 2-band AGC settings." << std::endl;
            
            YAML::Node hf_enhance = yaml_node["hf_enhance"];
            enable_hf_enhance = hf_enhance["enabled"].as<bool>();
            
            YAML::Node mb_crossover = yaml_node["multiband_crossover"];
            enable_mb_crossover = mb_crossover["enabled"].as<bool>();
            
            /* Read multiband params */
                        
            /* Read compressor bands */
            
            YAML::Node compressors = yaml_node["multiband_params"]["compressor"];
            
            enable_mb_compressor = compressors["enabled"].as<bool>();
            
            auto comp_bands = compressors["bands"];
            
            uint16_t current_band = 0;
            COMPRESSOR_PARAMS comp_params;
            
            for(YAML::iterator band_it = comp_bands.begin(); band_it != comp_bands.end(); ++band_it) {
                                
                for(YAML::iterator band_details_it = (*band_it).begin(); band_details_it != (*band_it).end(); ++band_details_it) {
                    uint16_t band_num = band_details_it->first.as<uint16_t>();
                    auto &band_map = band_details_it->second;
                                    
                    for (const auto& key_value : band_map) {
                        // Now 'key_value' is a key/value pair, so you can read it:
                        std::string key = key_value.first.as<std::string>();
                        YAML::Node value = key_value.second;
                        //std::cout << key << ": " << value.as<std::string>() << std::endl;
                        
                        if(!key.compare("band")) {
                            current_band = value.as<uint16_t>();
                        }
                        else if(!key.compare("enabled")) {
                            comp_params.enabled = value.as<bool>();
                        }
                        else if(!key.compare("mute")) {
                            comp_params.mute = value.as<bool>();
                        }
                        else if(!key.compare("drive")) {
                            comp_params.drive = value.as<float>();
                        }
                        else if(!key.compare("release")) {
                            comp_params.release = value.as<float>();
                        }
                        else if(!key.compare("gate_thresh")) {
                            comp_params.gate_thresh = value.as<float>();
                        }
                        else if(!key.compare("use_coupling")) {
                            comp_params.use_coupling = value.as<bool>();
                        }
                        else if(!key.compare("coupling")) {
                            comp_params.coupling = value.as<float>();
                        }
                        else if(!key.compare("window_size")) {
                            comp_params.window_size = value.as<float>();
                        }
                        else if(!key.compare("window_release")) {
                            comp_params.window_release = value.as<float>();
                        }
                        else if(!key.compare("ratio")) {
                            comp_params.ratio = value.as<float>();
                        }
                        else if(!key.compare("idle_gain")) {
                            comp_params.idle_gain = value.as<float>();
                        }
                        else if(!key.compare("attack")) {
                            comp_params.attack = value.as<float>();
                        }
                        else if(!key.compare("post_gain")) {
                            comp_params.post_gain = value.as<float>();
                        }
                        else {
                            //unknown cfg element in AGC params
                            std::cout << "UNKNOWN ELEMENT IN COMPRESSOR PARAMS" << std::endl;
                        }
                    }
                }
                
                multiband_params.comp_params[current_band - 1] = comp_params;
                std::cout << "Read compressor params for band " << current_band << std::endl;
                
            }
            
            /* Read Limiter Bands */
            YAML::Node limiters = yaml_node["multiband_params"]["limiter"];
            
            enable_mb_limiter = limiters["enabled"].as<bool>();
            
            auto lim_bands = limiters["bands"];
            
            current_band = 0;
            COMPRESSOR_PARAMS lim_params;
                        
            for(YAML::iterator band_it = lim_bands.begin(); band_it != lim_bands.end(); ++band_it) {
                                
                for(YAML::iterator band_details_it = (*band_it).begin(); band_details_it != (*band_it).end(); ++band_details_it) {
                    uint16_t band_num = band_details_it->first.as<uint16_t>();
                    auto &band_map = band_details_it->second;
                                    
                    for (const auto& key_value : band_map) {
                        
                        // Now 'key_value' is a key/value pair, so you can read it:
                        std::string key = key_value.first.as<std::string>();
                        YAML::Node value = key_value.second;
                        //std::cout << key << ": " << value.as<std::string>() << std::endl;
                        
                        if(!key.compare("band")) {
                            current_band = value.as<uint16_t>();
                        }
                        else if(!key.compare("enabled")) {
                            lim_params.enabled = value.as<bool>();
                        }
                        else if(!key.compare("mute")) {
                            lim_params.mute = value.as<bool>();
                        }
                        else if(!key.compare("drive")) {
                            lim_params.drive = value.as<float>();
                        }
                        else if(!key.compare("release")) {
                            lim_params.release = value.as<float>();
                        }
                        else if(!key.compare("gate_thresh")) {
                            lim_params.gate_thresh = value.as<float>();
                        }
                        else if(!key.compare("use_coupling")) {
                            lim_params.use_coupling = value.as<bool>();
                        }
                        else if(!key.compare("coupling")) {
                            lim_params.coupling = value.as<float>();
                        }
                        else if(!key.compare("window_size")) {
                            lim_params.window_size = value.as<float>();
                        }
                        else if(!key.compare("window_release")) {
                            lim_params.window_release = value.as<float>();
                        }
                        else if(!key.compare("ratio")) {
                            lim_params.ratio = value.as<float>();
                        }
                        else if(!key.compare("idle_gain")) {
                            lim_params.idle_gain = value.as<float>();
                        }
                        else if(!key.compare("attack")) {
                            lim_params.attack = value.as<float>();
                        }
                        else if(!key.compare("post_gain")) {
                            lim_params.post_gain = value.as<float>();
                        }
                        else {
                            //unknown cfg element in limiter params
                            std::cout << "UNKNOWN ELEMENT IN LIMITER PARAMS" << std::endl;
                        }
                    }
                }
                multiband_params.lim_params[current_band - 1] = lim_params;
                std::cout << "Read limiter params for band " << current_band << std::endl;
            }
        }
    }
    catch(const std::exception& ex) {
        std::cout << "CONFIG FILE READ FAILED: " << ex.what() << std::endl;
        return false;
    }
    
    return true;
}
}


/*  This was pulled from the AGCControlsView source file.
 *  just parking it here in case these are good settings.
AGC_PARAMS factory_agc_params = {
    .enabled = true,
    .mute_lo = false,
    .mute_hi = false,
    .drive = -40.0,
    .release_master = 0.542,
    .release_bass = 0.542,
    .gate_thresh = -35.0,
    .bass_coupling = 0.3,
    .window_size = -3.0,
    .window_release = 60,
    .ratio = 200.0,
    .bass_thresh = 0.0,
    .idle_gain = 0.0,
    .attack_master = 8.685,
    .attack_bass = 8.685,
    .post_gain = 17.0
};
*/
