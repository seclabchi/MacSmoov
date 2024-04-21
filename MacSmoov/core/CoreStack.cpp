//
//  CoreStack.cpp
//  MacSmoov
//
//  Created by Paul Zaremba on 4/13/24.
//

#include "CoreStack.hpp"

namespace fmsmoov {

static CoreStack* theInstance = nullptr;

CoreStack* CoreStack::getInstance() {
    if(nullptr == theInstance) {
        theInstance = new CoreStack();
    }
    
    return theInstance;
}

CoreStack::CoreStack() {
    module_stack = new std::vector<fmsmoov::ProcessorModule*>();
    first_module = nullptr;
    last_module = nullptr;
    stack_size = 0;
}

CoreStack::~CoreStack() {
    for(ProcessorModule* mod : *module_stack) {
        delete mod;
    }
    
    delete module_stack;
}

void CoreStack::process(float* in_L, float* in_R, float* out_L, float* out_R, uint32_t n_samp) {
    /*
     * Connect the input buffers to the first module input,
     * and the output buffers to the last module output.
     */
    
    if(stack_size > 0){
        first_module->update_in_buf_ref(0, in_L);
        first_module->update_in_buf_ref(1, in_R);
        last_module->update_out_buf_ref(0, out_L);
        last_module->update_out_buf_ref(1, out_R);
    }
    
    for(ProcessorModule* mod : *module_stack) {
        mod->process();
    }
}

void CoreStack::add_module(ProcessorModule* module) {
    module_stack->push_back(module);
    stack_size = module_stack->size();
    first_module = module_stack->at(0);
    last_module = module_stack->at(stack_size-1);
}

bool CoreStack::is_ready() {
    for(ProcessorModule* mod : *module_stack) {
        if(false == mod->is_ready()) {
            return false;
        }
    }
    
    return true;
}

}
