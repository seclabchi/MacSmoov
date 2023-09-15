#include "filter_lr4.h"
#include <string.h>
#include <iostream>

using namespace std;

FilterLR4::FilterLR4(SOS* _sos, uint32_t _n_samp) : m_nsamp(_n_samp), sos(_sos)
{
    //  cout << "Creating new biquad with params " << s1->g << " " << s1->b0 << " " << s1->b1 << " " << s1->b2 << " " << s1->a1 << " " << s1->a2 << endl;
    bq1 = new Biquad(sos);
    //  cout << "Creating new biquad with params " << s2->g << " " << s2->b0 << " " << s2->b1 << " " << s2->b2 << " " << s2->a1 << " " << s2->a2 << endl;
    bq2 = new Biquad(sos);
    tmpbuf = new float[m_nsamp]();
    memset(tmpbuf, 0, m_nsamp*sizeof(float));
}

FilterLR4::~FilterLR4()
{
    cout << "FilterLR4 DTOR" << endl;
	delete bq1;
	delete bq2;
	delete[] tmpbuf;
}

void FilterLR4::process(float* in, float* out)
{
	//memcpy(tmpbuf1, in, m_nsamp * sizeof(float));
    bq1->process(in, tmpbuf, m_nsamp);
    bq2->process(tmpbuf, out, m_nsamp);
}

AllpassFilterLR4::AllpassFilterLR4(SOS* _soslo, SOS* _soshi, uint32_t _n_samp) : m_nsamp(_n_samp)
{
	filterL = new FilterLR4(_soslo, m_nsamp);
	filterH = new FilterLR4(_soshi, m_nsamp);
	tmpbufL = new float[m_nsamp]();
    memset(tmpbufL, 0, m_nsamp*sizeof(float));
	tmpbufH = new float[m_nsamp]();
    memset(tmpbufH, 0, m_nsamp*sizeof(float));
}

AllpassFilterLR4::~AllpassFilterLR4()
{
    cout << "AllpassFilterLR4 DTOR" << endl;
	delete filterL;
	delete filterH;
	delete[] tmpbufL;
	delete[] tmpbufH;
}

void AllpassFilterLR4::process(float* in, float* out)
{
	filterL->process(in, tmpbufL);
	filterH->process(in, tmpbufH);

	for(uint32_t i = 0; i < m_nsamp; i++)
	{
		out[i] = tmpbufL[i] + tmpbufH[i];
	}
}
