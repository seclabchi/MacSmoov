#include "filter_butter3rd.h"
#include <string.h>
#include <iostream>

using namespace std;

FilterButter3rd::FilterButter3rd(uint32_t _n_samps) : n_samps(_n_samps)
{
    butter = new tonekids::dsp::Butterworth();
}

FilterButter3rd::~FilterButter3rd()
{
    delete butter;
}

void FilterButter3rd::process(float* in, float* out)
{
    
}
