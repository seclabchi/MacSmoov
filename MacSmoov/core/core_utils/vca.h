#ifndef __VCA_H__
#define __VCA_H__

#include <unistd.h>
#include <stdint.h>

#include "LogLinConverter.hpp"

namespace fmsmoov {

/* We're making the general VCA architecture purely virtual because there are different control
   algorithms that we want to use (purely proportional, declinear, etc.)
 */

typedef enum {
  LINEAR = 0,
  DECILINEAR = 1
} VCA_TYPE;

class VCA
{
public:
    VCA(uint32_t samprate, uint32_t _n_samps);
    virtual void config(VCA_TYPE _type);
	virtual ~VCA();
    virtual void process(float* input, float* control, float* output);

protected:
    uint32_t samprate, n_samps;
private:
    VCA();
    VCA_TYPE type;
    float* tmp_buf;
};

}
#endif //#ifndef __VCA_H__
