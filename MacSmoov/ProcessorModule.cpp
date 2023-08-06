//
//  ProcessorModule.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 7/9/23.
//

#include "ProcessorModule.hpp"

namespace fmsmoov {

ProcessorModule::ProcessorModule(uint32_t _f_samp, uint32_t _n_channels, uint32_t _buf_size) : f_samp(_f_samp), n_channels(_n_channels),
    buf_size(_buf_size) {
    
}

ProcessorModule::~ProcessorModule() {
    
}

uint32_t ProcessorModule::get_f_samp() {
    return f_samp;
}

uint32_t ProcessorModule::get_n_channels() {
    return n_channels;
}

uint32_t ProcessorModule::get_buf_size() {
    return buf_size;
}

}
