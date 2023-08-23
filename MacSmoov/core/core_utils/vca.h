#ifndef __VCA_H__
#define __VCA_H__

#include <unistd.h>
#include <stdint.h>

#include "LogLinConverter.hpp"

namespace fmsmoov {

/* We're making the general VCA architecture purely virtual because there are different control
   algorithms that we want to use (purely proportional, declinear, etc.)
 */

class VCA
{
public:
    VCA(uint32_t samprate, uint32_t _n_samps);
    VCA(const VCA& rhs);
	virtual ~VCA();
    virtual void process(float* in, float* control, float* out, uint32_t n_samps) = 0;

protected:
    uint32_t samprate, n_samps;
private:
    VCA();
};

}
#endif //#ifndef __VCA_H__
