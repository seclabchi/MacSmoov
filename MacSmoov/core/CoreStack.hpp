//
//  CoreStack.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 4/13/24.
//

#ifndef CoreStack_hpp
#define CoreStack_hpp

#include <stdio.h>
#include <iostream>
#include <vector>

#include "ProcessorModule.hpp"
#include "core_utils/channel_map.hpp"

using namespace std;

namespace fmsmoov {
class CoreStack {
public:
    static CoreStack* getInstance();
    virtual ~CoreStack();
    bool is_ready();
    void add_module(ProcessorModule* module);
    void set_external_bufs();
    void process(float* in_L, float* in_R, float* out_L, float* out_R, uint32_t n_samp);
private:
    CoreStack();
    uint8_t stack_size;
    ProcessorModule* first_module;
    ProcessorModule* last_module;
    vector<fmsmoov::ProcessorModule*>* module_stack;
};
}

#endif /* CoreStack_hpp */
