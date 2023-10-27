//
//  ProcModDelay.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 10/15/23.
//

#ifndef ProcModDelay_hpp
#define ProcModDelay_hpp

#include <stdio.h>
#include <stdint.h>
#include "ProcessorModule.hpp"

namespace fmsmoov {

class ProcModDelay : public ProcessorModule {
public:
    ProcModDelay(const string& _name, uint32_t _f_samp, uint8_t _n_channels, uint32_t _n_samps);
    virtual ~ProcModDelay();
    /* n samps is total interleaved stereo samples
     TODO: Figure this shit out to make it universal. */
    virtual void process();
    void set_delay(float _delay_ms);
private:
    float m_delay_ms;
    uint32_t m_delay_samp;
    float* m_bufL, *m_bufR;
    float* m_wpL, *m_wpR, *m_rpL, *m_rpR;
};

}

#endif /* ProcModDelay_hpp */
