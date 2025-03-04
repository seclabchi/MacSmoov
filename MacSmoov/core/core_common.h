//
//  core_common.h
//  MacSmoov
//
//  Created by Paul Zaremba on 8/28/23.
//

#ifndef core_common_h
#define core_common_h

typedef enum {
    SOFT_KNEE = 0,
    HARD_KNEE = 1
} COMPRESSOR_KNEE_TYPE;

typedef enum {
    AUTO = 0,
    MANUAL = 1
} MAKEUP_GAIN_MODE;

typedef struct {
    bool enabled;  //is the AGC in or out
    bool mute_lo;  //mute the low band
    bool mute_hi;  //mute the hi band
    float drive;  //amount of gain at AGC input
    float thresh; //threshold of AGC
    float release_master;  //0.5...20 dB/s
    float release_bass;    //1...10 dB/s
    float gate_thresh;  //-80...-15 dB
    float bass_coupling;  //0-100%
    float window_size;  //-25...0 dB
    float window_release;  //0.5...20 dB
    float ratio;  //infinity:1....2:1
    float bass_thresh;  //-12...2.5 dB
    float idle_gain;  //-10...+10 dB
    float attack_master;  //0.2...6 seconds
    float attack_bass;   //1...10 seconds
    COMPRESSOR_KNEE_TYPE bass_knee_type;
    float bass_knee_width;
    COMPRESSOR_KNEE_TYPE master_knee_type;
    float master_knee_width;
    MAKEUP_GAIN_MODE bass_makeup_gain_mode;
    float bass_makeup_gain;
    MAKEUP_GAIN_MODE master_makeup_gain_mode;
    float master_makeup_gain;
} AGC_PARAMS;

typedef struct  {
    float release;
    float thresh;
    float gate_thresh;
    float ratio;
    float attack;
    COMPRESSOR_KNEE_TYPE knee_type;
    float knee_width;
    float idle_gain;
    MAKEUP_GAIN_MODE makeup_gain_mode;
    float makeup_gain;
} COMPRESSOR_PARAMS;

typedef struct  {
    bool enabled;
    float drive;
    float gate_thresh;
    bool limiters_enabled;
    float master_post_gain;
    bool band1_solo;
    bool band1_mute;
    bool band2_solo;
    bool band2_mute;
    bool band3_solo;
    bool band3_mute;
    bool band4_solo;
    bool band4_mute;
    bool band5_solo;
    bool band5_mute;
    float band34_coupling;
    float band45_coupling;
    float band32_coupling;
    float band23_coupling;
    float band21_coupling;
    bool band1_compressor_enabled;
    bool band2_compressor_enabled;
    bool band3_compressor_enabled;
    bool band4_compressor_enabled;
    bool band5_compressor_enabled;
    bool band1_limiter_enabled;
    bool band2_limiter_enabled;
    bool band3_limiter_enabled;
    bool band4_limiter_enabled;
    bool band5_limiter_enabled;
    float band1_comp_lim_offset;
    float band2_comp_lim_offset;
    float band3_comp_lim_offset;
    float band4_comp_lim_offset;
    float band5_comp_lim_offset;
    COMPRESSOR_PARAMS comp_params[5];
    COMPRESSOR_PARAMS lim_params[5];
} MULTIBAND_PARAMS;


#endif /* core_common_h */
