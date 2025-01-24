#include "filter_el.h"
#include <string.h>
#include <iostream>

using namespace std;

FilterEL::FilterEL(const tonekids::dsp::SOS& _sos1, const tonekids::dsp::SOS& _sos2, const tonekids::dsp::SOS& _sos3, uint32_t _n_samp) : sos1(_sos1), sos2(_sos2), sos3(_sos3), m_nsamp(_n_samp)
{
    bq1 = new tonekids::dsp::Biquad(sos1, 0.0f);
    bq2 = new tonekids::dsp::Biquad(sos2, 0.0f);
    bq3 = new tonekids::dsp::Biquad(sos3, 0.0f);
    tmpbuf1 = new float[m_nsamp]();
    tmpbuf2 = new float[m_nsamp]();
}

FilterEL::~FilterEL()
{
	delete bq1;
    delete bq2;
    delete bq3;
    delete[] tmpbuf1;
    delete[] tmpbuf2;
}

void FilterEL::process(float* in, float* out)
{
    bq1->process(in, tmpbuf1, m_nsamp);
    bq2->process(tmpbuf1, tmpbuf2, m_nsamp);
    bq3->process(tmpbuf2, out, m_nsamp);
}
