//
//  CoreConfig.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 4/13/24.
//

#include <stdio.h>
#include "CoreConfig.h"

namespace fmsmoov {

static CoreConfig* the_instance = nullptr;

/*  DEFAULT FALLBACK FACTORY SETTINGS.  These are loaded if a config file is not found. */
static COMPRESSOR_PARAMS comp_params_b1 = {
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
 
static COMPRESSOR_PARAMS comp_params_b2 = {
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
 
static COMPRESSOR_PARAMS comp_params_b3 = {
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
 
static COMPRESSOR_PARAMS comp_params_b4 = {
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
 
static COMPRESSOR_PARAMS comp_params_b5 = {
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


static COMPRESSOR_PARAMS lim_params_b1 = {
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
 
static COMPRESSOR_PARAMS lim_params_b2 = {
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
 
static COMPRESSOR_PARAMS lim_params_b3 = {
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
 
static COMPRESSOR_PARAMS lim_params_b4 = {
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
 
static COMPRESSOR_PARAMS lim_params_b5 = {
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
 
 
static MULTIBAND_PARAMS multiband_params_default = {
     .comp_params[0] = comp_params_b1,
     .lim_params[0] = lim_params_b1,
     .comp_params[1] = comp_params_b2,
     .lim_params[1] = lim_params_b2,
     .comp_params[2] = comp_params_b3,
     .lim_params[2] = lim_params_b3,
     .comp_params[3] = comp_params_b4,
     .lim_params[3] = lim_params_b4,
     .comp_params[4] = comp_params_b5,
     .lim_params[4] = lim_params_b5
 };
 


CoreConfig::CoreConfig() {
    this->multiband_params = multiband_params_default;
}

CoreConfig* CoreConfig::get_instance() {
    if(nullptr == the_instance) {
        the_instance = new CoreConfig();
    }
    
    return the_instance;
}

}
