//
//  ProcModLogLinConverter.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 8/7/23.
//

#include "LogLinConverter.hpp"

namespace fmsmoov {

LogLinConverter::LogLinConverter(SAMP_CONV_TYPE _type) : m_type(_type) {
    
}

LogLinConverter::~LogLinConverter() {
    
}

void LogLinConverter::set_conv_type(SAMP_CONV_TYPE type) {
    m_type = type;
}

void LogLinConverter::process(float* in, float* out, uint32_t n_samps) {
    switch(m_type) {
    case LIN_TO_LOG:
            for(uint32_t i = 0; i < n_samps; i++) {
                out[i] = 20*log10f(in[i]);
            }
        break;
    case LOG_TO_LIN:
            for(uint32_t i = 0; i < n_samps; i++) {
                out[i] = powf(10, in[i]/20.0);
            }
        break;
    }
}

}
