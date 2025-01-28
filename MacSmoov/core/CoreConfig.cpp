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
    .enabled = true,
    .mute_lo = false,
    .mute_hi = false,
    .drive = 10.0,
    .target = -24.0,
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
};

/*===================================*/
/* MULTIBAND COMPRESSORS             */
/*===================================*/

static const float multiband_drive = 10.0f;
static const float multiband_gate_thresh = -30.0f;

static const COMPRESSOR_PARAMS comp_params_b1_default = {
    .target = -24.0,
    .release = .100,
    .thresh = multiband_gate_thresh,
    .ratio = 1.5,
    .attack = 0.100
 };
 
static const COMPRESSOR_PARAMS comp_params_b2_default = {
    .target = -24.0,
    .release = .100,
    .thresh = multiband_gate_thresh,
    .ratio = 1.5,
    .attack = 0.100

 };
 
static const COMPRESSOR_PARAMS comp_params_b3_default = {
    .target = -24.0,
    .release = .100,
    .thresh = multiband_gate_thresh,
    .ratio = 1.5,
    .attack = 0.100
 };
 
static const COMPRESSOR_PARAMS comp_params_b4_default = {
    .target = -24.0,
    .release = .100,
    .thresh = multiband_gate_thresh,
    .ratio = 1.5,
    .attack = 0.100
 };
 
static const COMPRESSOR_PARAMS comp_params_b5_default = {
    .target = -24.0,
    .release = .100,
    .thresh = multiband_gate_thresh,
    .ratio = 1.5,
    .attack = 0.100
 };
 
 /*===================================*/
 /* LIMITERS                          */
 /*===================================*/


static const COMPRESSOR_PARAMS lim_params_b1_default = {
    .target = -24.0,
    .release = .020,
    .thresh = multiband_gate_thresh,
    .ratio = 10,
    .attack = .020
 };
 
static const COMPRESSOR_PARAMS lim_params_b2_default = {
    .target = -24.0,
    .release = .020,
    .thresh = multiband_gate_thresh,
    .ratio = 10,
    .attack = .020
 };
 
static const COMPRESSOR_PARAMS lim_params_b3_default = {
    .target = -24.0,
    .release = .020,
    .thresh = multiband_gate_thresh,
    .ratio = 10,
    .attack = .020
 };
 
static const COMPRESSOR_PARAMS lim_params_b4_default = {
    .target = -24.0,
    .release = .020,
    .thresh = multiband_gate_thresh,
    .ratio = 10,
    .attack = .020
 };
 
static const COMPRESSOR_PARAMS lim_params_b5_default = {
    .target = -24.0,
    .release = .020,
    .thresh = multiband_gate_thresh,
    .ratio = 10,
    .attack = .020
 };
 
 
static const MULTIBAND_PARAMS multiband_params_default = {
    .enabled = true,
    .band1_solo = false,
    .band1_mute = false,
    .band2_solo = false,
    .band2_mute = false,
    .band3_solo = false,
    .band3_mute = false,
    .band4_solo = false,
    .band4_mute = false,
    .band5_solo = false,
    .band5_mute = false,
    .band1_compressor_enabled = true,
    .band2_compressor_enabled = true,
    .band3_compressor_enabled = true,
    .band4_compressor_enabled = true,
    .band5_compressor_enabled = true,
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
    agc_params = agc_params_default;
    enable_hf_enhance = false;
    enable_mb_crossover = false;
    multiband_params = multiband_params_default;
}

CoreConfig* CoreConfig::get_instance() {
    if(nullptr == the_instance) {
        the_instance = new CoreConfig();
    }
    
    return the_instance;
}

void CoreConfig::set_input_gain_enabled(bool enabled) {
    enable_input_gain = enabled;
}

bool CoreConfig::get_input_gain_enabled() {
    return enable_input_gain;
}

void CoreConfig::get_input_gain(std::pair<float, float>& gain) {
    gain = input_gain;
}

void CoreConfig::set_input_gain(const std::pair<float, float>& gain) {
    input_gain = gain;
    write_cfg_to_file();
}

bool CoreConfig::get_input_level_enabled() {
    return enable_input_level;
}

void CoreConfig::set_input_level_enabled(bool enabled) {
    enable_input_level = enabled;
}

bool CoreConfig::get_2band_agc_enabled() {
    return agc_params.enabled;
}

void CoreConfig::set_2band_agc_enabled(bool _enabled) {
    agc_params.enabled = _enabled;
}

void CoreConfig::get_agc_params(AGC_PARAMS& params) {
    params = agc_params;
}

bool CoreConfig::set_agc_params(const AGC_PARAMS& params) {
    agc_params = params;
    write_cfg_to_file();
    return true;
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
    return multiband_params.enabled;
}

void CoreConfig::set_mb_compressor_enabled(bool enabled) {
    multiband_params.enabled = enabled;
}

bool CoreConfig::get_mb_limiter_enabled() {
    return false;
}

void CoreConfig::set_mb_limiter_enabled(bool enabled) {
    //enable_mb_limiter = enabled;
    //TODO: get the limiter enable functionality implemented
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
    
    cfg_file_name = filename;
    std::ifstream cfg_file_stream;
    
    try {
        std::cout << "Reading config from " << filename << std::endl;
        
        const char* cfg_file_path = filename.c_str();
        cfg_file_stream.open(cfg_file_path);
        if(cfg_file_stream.fail()) {
            std::cout << "Can't open config file for reasons..." << strerror(errno) << std::endl;
        }
        else {
            yaml_node = YAML::LoadFile(cfg_file_path);
            
            std::cout << "Config YAML Dump:\n=================\n" << YAML::Dump(yaml_node).c_str() << std::endl;
            
            YAML::Node input_gain_node = yaml_node["input_gain"];
            enable_input_gain = input_gain_node["enabled"].as<bool>();
            float input_gain_L = input_gain_node["L"].as<float>();
            float input_gain_R = input_gain_node["R"].as<float>();
            input_gain = std::make_pair(input_gain_L, input_gain_R);
            
            YAML::Node input_level = yaml_node["input_level"];
            enable_input_level = input_level["enabled"].as<bool>();
            
            YAML::Node stereo_enhance_node = yaml_node["stereo_enhance"];
            enable_stereo_enhance = stereo_enhance_node["enabled"].as<bool>();
            
            std::cout << "Reading AGC Params..." << std::endl;
            
            /* Read AGC Params */
            YAML::Node agc = yaml_node["agc_params"];
            agc_params.enabled = agc["enabled"].as<bool>();
            agc_params.mute_lo = agc["mute_lo"].as<bool>();
            agc_params.mute_hi = agc["mute_hi"].as<bool>();
            agc_params.drive = agc["drive"].as<float>();
            agc_params.target = agc["target"].as<float>();
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
        
            std::cout << "Loaded 2-band AGC settings." << std::endl;
            
            YAML::Node hf_enhance = yaml_node["hf_enhance"];
            enable_hf_enhance = hf_enhance["enabled"].as<bool>();
            
            YAML::Node mb_crossover = yaml_node["multiband_crossover"];
            enable_mb_crossover = mb_crossover["enabled"].as<bool>();
            
            /* Read multiband params */
            
            YAML::Node mb_params = yaml_node["multiband_params"];
            
            multiband_params.enabled = mb_params["enabled"].as<bool>();
            multiband_params.drive = mb_params["drive"].as<float>();
            multiband_params.gate_thresh = mb_params["gate_thresh"].as<float>();
            multiband_params.band1_solo = mb_params["band1_solo"].as<bool>();
            multiband_params.band1_mute = mb_params["band1_mute"].as<bool>();
            multiband_params.band2_solo = mb_params["band2_solo"].as<bool>();
            multiband_params.band2_mute = mb_params["band2_mute"].as<bool>();
            multiband_params.band3_solo = mb_params["band3_solo"].as<bool>();
            multiband_params.band3_mute = mb_params["band3_mute"].as<bool>();
            multiband_params.band4_solo = mb_params["band4_solo"].as<bool>();
            multiband_params.band4_mute = mb_params["band4_mute"].as<bool>();
            multiband_params.band5_solo = mb_params["band5_solo"].as<bool>();
            multiband_params.band5_mute = mb_params["band5_mute"].as<bool>();
            
            /* Read compressor bands */
            
            YAML::Node compressors = yaml_node["multiband_params"]["compressors"];
            
            multiband_params.band1_compressor_enabled = compressors["band1_compressor_enabled"].as<bool>();
            multiband_params.band2_compressor_enabled = compressors["band2_compressor_enabled"].as<bool>();
            multiband_params.band3_compressor_enabled = compressors["band3_compressor_enabled"].as<bool>();
            multiband_params.band4_compressor_enabled = compressors["band4_compressor_enabled"].as<bool>();
            multiband_params.band5_compressor_enabled = compressors["band5_compressor_enabled"].as<bool>();
            multiband_params.band34_coupling = compressors["band34_coupling"].as<float>();
            multiband_params.band45_coupling = compressors["band45_coupling"].as<float>();
            multiband_params.band32_coupling = compressors["band32_coupling"].as<float>();
            multiband_params.band23_coupling = compressors["band23_coupling"].as<float>();
            multiband_params.band21_coupling = compressors["band21_coupling"].as<float>();
            
            auto comp_bands = compressors["bands"];

            COMPRESSOR_PARAMS comp_params;
            
            for(std::size_t band_num = 0; band_num < comp_bands.size(); band_num++) {
                
                std::cout << "Reading compressor band " << band_num + 1 << " settings..." << std::endl;
                
                YAML::Node band_settings = comp_bands[band_num];
                
                for(YAML::iterator band_settings_it = band_settings.begin(); band_settings_it != band_settings.end(); ++band_settings_it) {
                    std::string key = band_settings_it->first.as<std::string>();
                    YAML::Node value = band_settings_it->second;
                    
                    std::cout << "Found setting " << key << std::endl;
                    
                    if(!key.compare("band")) {
                        //don't care, just for human readable purposes
                    }
                    else if(!key.compare("target")) {
                        comp_params.target = value.as<float>();
                    }
                    else if(!key.compare("release")) {
                        comp_params.release = value.as<float>();
                    }
                    else if(!key.compare("ratio")) {
                        comp_params.ratio = value.as<float>();
                    }
                    else if(!key.compare("attack")) {
                        comp_params.attack = value.as<float>();
                    }
                    else {
                        //unknown cfg element in COMPRESSOR params
                        std::cout << "UNKNOWN ELEMENT " << key << " IN COMPRESSOR PARAMS" << std::endl;
                    }
                }
                
                multiband_params.comp_params[band_num] = comp_params;
                std::cout << "Read compressor params for band " << band_num + 1 << std::endl;
                
            }
            
            /* Need to populate each compressor band gate threshold based on upper-level config.  This is probably
              * inefficient and might need to change later.
              */
            multiband_params.comp_params[0].thresh = mb_params["gate_thresh"].as<float>();
            multiband_params.comp_params[1].thresh = mb_params["gate_thresh"].as<float>();
            multiband_params.comp_params[2].thresh = mb_params["gate_thresh"].as<float>();
            multiband_params.comp_params[3].thresh = mb_params["gate_thresh"].as<float>();
            multiband_params.comp_params[4].thresh = mb_params["gate_thresh"].as<float>();
            
            
            /* Read Limiter Bands */
            YAML::Node limiters = yaml_node["multiband_params"]["limiters"];
            
            multiband_params.band1_limiter_enabled = limiters["band1_limiter_enabled"].as<bool>();
            multiband_params.band2_limiter_enabled = limiters["band2_limiter_enabled"].as<bool>();
            multiband_params.band3_limiter_enabled = limiters["band3_limiter_enabled"].as<bool>();
            multiband_params.band4_limiter_enabled = limiters["band4_limiter_enabled"].as<bool>();
            multiband_params.band5_limiter_enabled = limiters["band5_limiter_enabled"].as<bool>();
            
            auto lim_bands = limiters["bands"];
            
            COMPRESSOR_PARAMS lim_params;
            
            for(std::size_t band_num = 0; band_num < lim_bands.size(); band_num++) {
                
                std::cout << "Reading limiter band " << band_num + 1 << " settings..." << std::endl;
                
                YAML::Node band_settings = lim_bands[band_num];
                
                for(YAML::iterator band_settings_it = band_settings.begin(); band_settings_it != band_settings.end(); ++band_settings_it) {
                    std::string key = band_settings_it->first.as<std::string>();
                    YAML::Node value = band_settings_it->second;
                    
                    std::cout << "Found setting " << key << std::endl;
                    
                    if(!key.compare("band")) {
                        //don't care, just for human readable purposes
                    }
                    else if(!key.compare("target")) {
                        lim_params.target = value.as<float>();
                    }
                    else if(!key.compare("release")) {
                        lim_params.release = value.as<float>();
                    }
                    else if(!key.compare("ratio")) {
                        lim_params.ratio = value.as<float>();
                    }
                    else if(!key.compare("attack")) {
                        lim_params.attack = value.as<float>();
                    }
                    else {
                        //unknown cfg element in COMPRESSOR params
                        std::cout << "UNKNOWN ELEMENT " << key << " IN COMPRESSOR PARAMS" << std::endl;
                    }
                }
                
                multiband_params.lim_params[band_num] = lim_params;
                std::cout << "Read limiter params for band " << band_num + 1 << std::endl;
                
            }
        }
        
        cfg_file_stream.close();
    }
    catch(const std::exception& ex) {
        std::cout << "CONFIG FILE READ FAILED: " << ex.what() << std::endl;
        cfg_file_stream.close();
        return false;
    }
    
    return true;
}

bool CoreConfig::write_cfg_to_file() {
    
    std::ofstream cfg_file_stream;
    
    try {
        std::cout << "Writing config to " << cfg_file_name << std::endl;
        
        //std::cout << "Config YAML Dump:\n=================\n" << YAML::Dump(yaml_node).c_str() << std::endl;
        
        const char* cfg_file_path = cfg_file_name.c_str();
        cfg_file_stream.open(cfg_file_path);
        if(cfg_file_stream.fail()) {
            std::cout << "Can't open config file for reasons..." << strerror(errno) << std::endl;
        }
        else {
            YAML::Node input_gain_node = yaml_node["input_gain"];
            input_gain_node["enabled"] = enable_input_gain;
            input_gain_node["L"] = input_gain.first;
            input_gain_node["R"] = input_gain.second;
            
            YAML::Node input_level = yaml_node["input_level"];
            input_level["enabled"] = enable_input_level;
            
            YAML::Node stereo_enhance = yaml_node["stereo_enhance"];
            stereo_enhance["enabled"] = enable_stereo_enhance;
                        
            /* Read AGC Params */
            YAML::Node agc = yaml_node["agc_params"];
            agc["enabled"] = agc_params.enabled;
            agc["mute_lo"] = agc_params.mute_lo;
            agc["mute_hi"] = agc_params.mute_hi;
            agc["drive"] = agc_params.drive;
            agc["target"] = agc_params.target;
            agc["release_master"] = agc_params.release_master;
            agc["release_bass"] = agc_params.release_bass;
            agc["gate_thresh"] = agc_params.gate_thresh;
            agc["bass_coupling"] = agc_params.bass_coupling;
            agc["window_size"] = agc_params.window_size;
            agc["window_release"] = agc_params.window_release;
            agc["ratio"] = agc_params.ratio;
            agc["bass_thresh"] = agc_params.bass_thresh;
            agc["idle_gain"] = agc_params.idle_gain;
            agc["attack_master"] = agc_params.attack_master;
            agc["attack_bass"] = agc_params.attack_bass;
        
            std::cout << "Saved 2-band AGC settings." << std::endl;
            
            YAML::Node hf_enhance = yaml_node["hf_enhance"];
            hf_enhance["enabled"] = enable_hf_enhance;
            
            YAML::Node mb_crossover = yaml_node["multiband_crossover"];
            mb_crossover["enabled"] = enable_mb_crossover;
            
            /* Write multiband params */
            
            YAML::Node multiband_params_node = yaml_node["multiband_params"];
            
            multiband_params_node["enabled"] = multiband_params.enabled;
            multiband_params_node["drive"] = multiband_params.drive;
            multiband_params_node["gate_thresh"] = multiband_params.gate_thresh;
            multiband_params_node["band1_solo"] = multiband_params.band1_solo;
            multiband_params_node["band1_mute"] = multiband_params.band1_mute;
            multiband_params_node["band2_solo"] = multiband_params.band2_solo;
            multiband_params_node["band2_mute"] = multiband_params.band2_mute;
            multiband_params_node["band3_solo"] = multiband_params.band3_solo;
            multiband_params_node["band3_mute"] = multiband_params.band3_mute;
            multiband_params_node["band4_solo"] = multiband_params.band4_solo;
            multiband_params_node["band4_mute"] = multiband_params.band4_mute;
            multiband_params_node["band5_solo"] = multiband_params.band5_solo;
            multiband_params_node["band5_mute"] = multiband_params.band5_mute;
            
            YAML::Node compressors_node = multiband_params_node["compressors"];
            compressors_node["band1_compressor_enabled"] = multiband_params.band1_compressor_enabled;
            compressors_node["band2_compressor_enabled"] = multiband_params.band2_compressor_enabled;
            compressors_node["band3_compressor_enabled"] = multiband_params.band3_compressor_enabled;
            compressors_node["band4_compressor_enabled"] = multiband_params.band4_compressor_enabled;
            compressors_node["band5_compressor_enabled"] = multiband_params.band5_compressor_enabled;
            compressors_node["band34_coupling"] = multiband_params.band34_coupling;
            compressors_node["band45_coupling"] = multiband_params.band45_coupling;
            compressors_node["band32_coupling"] = multiband_params.band32_coupling;
            compressors_node["band23_coupling"] = multiband_params.band23_coupling;
            compressors_node["band21_coupling"] = multiband_params.band21_coupling;
            
            YAML::Node comp_bands = compressors_node["bands"];
            
            /* Write compressor bands */
            
            for(std::size_t band = 0; band < 5; band++) {
                YAML::Node band_settings;
                band_settings["band"] = band + 1;
                band_settings["target"] = multiband_params.comp_params[band].target;
                band_settings["release"] = multiband_params.comp_params[band].release;
                band_settings["ratio"] = multiband_params.comp_params[band].ratio;
                band_settings["attack"] = multiband_params.comp_params[band].attack;
                comp_bands[band] = band_settings;
            }
            
            YAML::Node limiters_node = multiband_params_node["limiters"];
            limiters_node["band1_limiter_enabled"] = multiband_params.band1_limiter_enabled;
            limiters_node["band2_limiter_enabled"] = multiband_params.band2_limiter_enabled;
            limiters_node["band3_limiter_enabled"] = multiband_params.band3_limiter_enabled;
            limiters_node["band4_limiter_enabled"] = multiband_params.band4_limiter_enabled;
            limiters_node["band5_limiter_enabled"] = multiband_params.band5_limiter_enabled;
            
            YAML::Node lim_bands = limiters_node["bands"];
            
            /* Write limiter bands */
            
            for(std::size_t band = 0; band < 5; band++) {
                YAML::Node band_settings;
                band_settings["band"] = band + 1;
                band_settings["target"] = multiband_params.lim_params[band].target;
                band_settings["release"] = multiband_params.lim_params[band].release;
                band_settings["ratio"] = multiband_params.lim_params[band].ratio;
                band_settings["attack"] = multiband_params.lim_params[band].attack;
                lim_bands[band] = band_settings;
            }
            
            YAML::Emitter emitter(cfg_file_stream);
            emitter << yaml_node;
        }
        cfg_file_stream.close();
    }
    catch(const std::exception& ex) {
        std::cout << "CONFIG FILE WRITE FAILED: " << ex.what() << std::endl;
        cfg_file_stream.close();
        return false;
    }
    
    return true;
}

} /* fmsmoov */




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
