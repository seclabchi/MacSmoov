#ifndef __VCA_PROPORTIONAL_H__
#define __VCA_PROPORTIONAL_H__

#include <unistd.h>
#include <stdint.h>

#include "vca.h"
#include "LogLinConverter.hpp"

namespace fmsmoov {


class VCAProportional : public VCA
{
public:
    VCAProportional(uint32_t samprate, uint32_t _n_samps);
    VCAProportional(const VCAProportional& rhs);
	virtual ~VCAProportional();
    virtual void process(float* in, float* control, float* out, uint32_t n_samps) = 0;

private:
    uint32_t samprate, n_samps;
};

}
#endif //#ifndef __VCA_PROPORTIONAL_H__
