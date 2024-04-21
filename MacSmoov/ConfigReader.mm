//
//  ConfigReader.m
//  MacSmoov
//
//  Created by Paul Zaremba on 3/24/24.
//

#import <Foundation/Foundation.h>
#import "ConfigReader.h"

#include <iostream>
#include <fstream>
#include <yaml-cpp/yaml.h>

using namespace std;

@implementation ConfigReader

static YAML::Node g_yaml_node;
static AGC_PARAMS agc_params;

- (id)init {
    
    NSLog(@"Initing ConfigReader...");
    
    self = [super init];
    
    NSLog(@"ConfigReader inited.");
    
    return self;
}

-(AGC_PARAMS) getAgcParams {
    return agc_params;
}

-(BOOL) readCfgFromFile:(NSString*)filename {
    try {
        NSLog(@"Reading config from %@...", filename);
        
        ifstream cfg_file_stream;
        const char* cfg_file_path = [filename UTF8String];
        cfg_file_stream.open(cfg_file_path);
        if(cfg_file_stream.fail()) {
            NSLog(@"Can't open config file for reasons...%s", strerror(errno));
        }
        else {
            g_yaml_node = YAML::LoadFile(cfg_file_path);
            auto type = g_yaml_node.Type();
            
            NSLog(@"Config YAML Dump:\n=================\n%s", YAML::Dump(g_yaml_node).c_str());
            
            /* Read AGC Params */
            YAML::Node agc = g_yaml_node["agc-params"];
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
            
            NSLog(@"Loaded 2-band AGC settings.");
            
        }
    }
    catch(const std::exception& ex) {
        NSLog(@"CONFIG FILE READ FAILED:  %s", ex.what());
        return false;
    }
    
    return true;
}

-(BOOL) writeCfgToFile:(NSString*)filename {
    return false;
}

@end
