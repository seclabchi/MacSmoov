//
//  LogLinConverter.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 8/7/23.
//

#ifndef LogLinConverter_hpp
#define LogLinConverter_hpp

#include <stdio.h>
#include <cstdint>
#include <math.h>

namespace fmsmoov {

typedef enum {
    LIN_TO_LOG,
    LOG_TO_LIN
} SAMP_CONV_TYPE;

class LogLinConverter {
public:
    LogLinConverter(SAMP_CONV_TYPE type);
    virtual ~LogLinConverter();
    virtual void process(float* in, float* out, uint32_t n_samps);
    void set_conv_type(SAMP_CONV_TYPE type);
private:
    SAMP_CONV_TYPE m_type;
};

}

#endif /* LogLinConverter_hpp */
