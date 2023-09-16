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

enum class LogLinConversionType {
    LIN_TO_LOG,
    LOG_TO_LIN
};

class LogLinConverter {
public:
    LogLinConverter(LogLinConversionType type);
    virtual ~LogLinConverter();
    virtual void process(float* in, float* out, uint32_t n_samps);
    void set_conv_type(LogLinConversionType type);
private:
    LogLinConversionType m_type;
};

}

#endif /* LogLinConverter_hpp */
