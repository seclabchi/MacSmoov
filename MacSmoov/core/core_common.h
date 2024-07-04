//
//  core_common.h
//  MacSmoov
//
//  Created by Paul Zaremba on 8/28/23.
//

#ifndef core_common_h
#define core_common_h


typedef struct {
    bool enabled;  //off-on
    bool mute_lo;  //mute the low band
    bool mute_hi;  //mute the hi band
    float drive;  //-10...+25 dB
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
    float post_gain;
} AGC_PARAMS;

typedef struct  {
    bool enabled;
    bool mute;
    bool solo;
    float drive;
    float release;
    float gate_thresh;
    bool use_coupling;
    float coupling;
    float window_size;
    float window_release;
    float ratio;
    float idle_gain;
    float attack;
    float post_gain;
} COMPRESSOR_PARAMS;

typedef struct  {
    COMPRESSOR_PARAMS comp_params[5];
    COMPRESSOR_PARAMS lim_params[5];
} MULTIBAND_PARAMS;


#endif /* core_common_h */
