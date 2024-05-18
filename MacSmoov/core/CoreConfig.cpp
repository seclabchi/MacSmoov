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

/*  DEFAULT FALLBACK FACTORY SETTINGS.  These are loaded if a config file is not found. */

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
    this->version = "0.0";
    this->cfg_name = "default";
    this->agc_params = agc_params_default;
    this->multiband_params = multiband_params_default;
}

CoreConfig* CoreConfig::get_instance() {
    if(nullptr == the_instance) {
        the_instance = new CoreConfig();
    }
    
    return the_instance;
}

void CoreConfig::get_agc_params(AGC_PARAMS& params) {
    params = agc_params;
}

void CoreConfig::get_mb_params(MULTIBAND_PARAMS& params) {
    params = multiband_params;
}

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
            
            //std::cout << "Config YAML Dump:\n=================\n" << YAML::Dump(g_yaml_node).c_str() << std::endl;
            
            /* Read AGC Params */
            YAML::Node agc = yaml_node["agc_params"];
            agc_params.enabled = agc["enabled"].as<bool>();
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
            
            /* Read multiband params */
            
            /* Read compressor bands */
            YAML::Node compressors = yaml_node["multiband_params"]["compressor"];
            YAML::Node comp_bands = compressors["bands"];
            
            uint16_t current_band = 0;
            COMPRESSOR_PARAMS comp_params;
            
            for (YAML::Node band : comp_bands) {
                //NSLog(@"%s", YAML::Dump(band).c_str());
                
                for (const auto& key_value : band) {
                    // Now 'key_value' is a key/value pair, so you can read it:
                    std::string key = key_value.first.as<std::string>();
                    YAML::Node value = key_value.second;
                    //std::cout << key << ": " << value << std::endl;
                    
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
                        comp_params.drive = value.as<double>();
                    }
                    else if(!key.compare("release")) {
                        comp_params.release = value.as<double>();
                    }
                    else if(!key.compare("gate_thresh")) {
                        comp_params.release = value.as<double>();
                    }
                    else if(!key.compare("use_coupling")) {
                        comp_params.mute = value.as<bool>();
                    }
                    else if(!key.compare("coupling")) {
                        comp_params.release = value.as<double>();
                    }
                    else if(!key.compare("window_size")) {
                        comp_params.release = value.as<double>();
                    }
                    else if(!key.compare("window_release")) {
                        comp_params.release = value.as<double>();
                    }
                    else if(!key.compare("ratio")) {
                        comp_params.release = value.as<double>();
                    }
                    else if(!key.compare("idle_gain")) {
                        comp_params.release = value.as<double>();
                    }
                    else if(!key.compare("attack")) {
                        comp_params.release = value.as<double>();
                    }
                    else if(!key.compare("post_gain")) {
                        comp_params.release = value.as<double>();
                    }
                    else {
                        //unknown cfg element in AGC params
                        std::cout << "UNKNOWN ELEMENT IN COMPRESSOR PARAMS" << std::endl;
                    }
                }
                
                multiband_params.comp_params[current_band] = comp_params;
                std::cout << "Read compressor params for band " << current_band << std::endl;
                
            }
            
            /* Read Limiter Bands */
            YAML::Node limiters = yaml_node["multiband_params"]["limiter"];
            YAML::Node lim_bands = limiters["bands"];
            
            COMPRESSOR_PARAMS lim_params;
            
            for (YAML::Node band : lim_bands) {
                //NSLog(@"%s", YAML::Dump(band).c_str());
                
                for (const auto& key_value : band) {
                    // Now 'key_value' is a key/value pair, so you can read it:
                    std::string key = key_value.first.as<std::string>();
                    YAML::Node value = key_value.second;
                    //std::cout << key << ": " << value << std::endl;
                    
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
                        lim_params.drive = value.as<double>();
                    }
                    else if(!key.compare("release")) {
                        lim_params.release = value.as<double>();
                    }
                    else if(!key.compare("gate_thresh")) {
                        lim_params.release = value.as<double>();
                    }
                    else if(!key.compare("use_coupling")) {
                        lim_params.mute = value.as<bool>();
                    }
                    else if(!key.compare("coupling")) {
                        lim_params.release = value.as<double>();
                    }
                    else if(!key.compare("window_size")) {
                        lim_params.release = value.as<double>();
                    }
                    else if(!key.compare("window_release")) {
                        lim_params.release = value.as<double>();
                    }
                    else if(!key.compare("ratio")) {
                        lim_params.release = value.as<double>();
                    }
                    else if(!key.compare("idle_gain")) {
                        lim_params.release = value.as<double>();
                    }
                    else if(!key.compare("attack")) {
                        lim_params.release = value.as<double>();
                    }
                    else if(!key.compare("post_gain")) {
                        lim_params.release = value.as<double>();
                    }
                    else {
                        //unknown cfg element in limiter params
                        std::cout << "UNKNOWN ELEMENT IN LIMITER PARAMS" << std::endl;
                    }
                }
                multiband_params.lim_params[current_band] = lim_params;
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
