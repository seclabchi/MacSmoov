#include "filter_el.h"
#include <string.h>
#include <iostream>

using namespace std;

FilterEL::FilterEL(SOS* _sos1, SOS* _sos2, SOS* _sos3, uint32_t _n_samp) : sos1(_sos1), sos2(_sos2), sos3(_sos3), m_nsamp(_n_samp)
{
    bq1 = new Biquad(sos1);
    bq2 = new Biquad(sos2);
    bq3 = new Biquad(sos3);
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
