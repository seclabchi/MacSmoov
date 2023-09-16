//
//  ProcModLogLinConverter.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 8/7/23.
//

#define EPS 0.0000001f

#include "LogLinConverter.hpp"

namespace fmsmoov {

LogLinConverter::LogLinConverter(LogLinConversionType _type) : m_type(_type) {
    
}

LogLinConverter::~LogLinConverter() {
    
}

void LogLinConverter::set_conv_type(LogLinConversionType type) {
    m_type = type;
}

void LogLinConverter::process(float* in, float* out, uint32_t n_samps) {
    switch(m_type) {
        case LogLinConversionType::LIN_TO_LOG:
            for(uint32_t i = 0; i < n_samps; i++) {
                out[i] = 20.0f*log10f(fabs(in[i]) + EPS);
            }
        break;
        case LogLinConversionType::LOG_TO_LIN:
            for(uint32_t i = 0; i < n_samps; i++) {
                out[i] = powf(10, in[i]/20.0);
            }
        break;
    }
}

}
