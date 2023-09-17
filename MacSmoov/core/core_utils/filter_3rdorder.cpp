#include "filter_3rdorder.h"
#include <string.h>
#include <iostream>

using namespace std;

Filter3rdOrder::Filter3rdOrder(SOS* _sos1, SOS* _sos2, uint32_t _n_samp) : sos1(_sos1), sos2(_sos2), m_nsamp(_n_samp)
{
    bq1 = new Biquad(sos1);
    bq2 = new Biquad(sos2);
    tmpbuf = new float[m_nsamp]();
}

Filter3rdOrder::~Filter3rdOrder()
{
	delete bq1;
    delete bq2;
    delete[] tmpbuf;
}

void Filter3rdOrder::process(float* in, float* out)
{
    bq1->process(in, tmpbuf, m_nsamp);
    bq2->process(tmpbuf, out, m_nsamp);
}
