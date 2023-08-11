//
//  ProcessorCore.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 4/15/23.
//

#include "ProcessorCore.hpp"
#include "LogLinConverter.hpp"
#include <string>


using namespace fmsmoov;

ProcessorCore::ProcessorCore(uint32_t _f_samp, uint32_t _n_channels, uint32_t _buf_size) : f_samp(_f_samp), n_channels(_n_channels), buf_size(_buf_size) {
    m_buf_samps = buf_size / sizeof(float);
    m_tmpbufA = new float[m_buf_samps];
    m_tmpbufB = new float[m_buf_samps];
    proc_mod_level_main_in = new ProcModLevelMeter(f_samp, n_channels, m_buf_samps);
    proc_mod_gain_main_in = new ProcModGain(f_samp, n_channels, m_buf_samps);
    
    m_loglin = new LogLinConverter(LOG_TO_LIN);
    m_linlog = new LogLinConverter(LIN_TO_LOG);
}

void ProcessorCore::process(float* in_samp, float* out_samp, uint32_t buf_samps) {
    //TODO: Assumes stereo interleaved float samples, and that n_bytes is the total size of the data buffer.
    //For now, just assume 2 channels, one float L and one float R.
    //memcpy(out_samp, in_samp, buf_samps * sizeof(float));
    
    proc_mod_gain_main_in->process(in_samp, m_tmpbufA, m_buf_samps);
    proc_mod_level_main_in->process(m_tmpbufA, out_samp, m_buf_samps);
}

void ProcessorCore::get_main_in_levels(float* lrms, float* rrms, float* lpeak, float* rpeak) {
    proc_mod_level_main_in->get_levels_db(lrms, rrms, lpeak, rpeak);
}

void ProcessorCore::set_main_in_gain_db(float loggain_l, float loggain_r) {
    proc_mod_gain_main_in->set_gain_db(loggain_l, loggain_r);
}
