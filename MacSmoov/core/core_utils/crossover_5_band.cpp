/*
 * Crossover_5_band.cpp
 *
 *  Created on: Aug 26, 2023
 *      Author: zaremba
 */

#include "crossover_5_band.h"
#include <iostream>

using namespace std;

/*
orban optimod-PC 1100 crossovers

 fcross =
    1800    520   6000    200

 ===========================================

 b0lo_sos =
    1.0000   2.0000   1.0000   1.0000  -1.6692   0.7166
 g0lo = 0.011858
 b0hi_sos =
    1.0000  -2.0000   1.0000   1.0000  -1.6692   0.7166
 g0hi = 0.8465
 
 ===========================================
 
 b1lo_sos =
    1.0000   2.0000   1.0000   1.0000  -1.9038   0.9082
 g1lo = 1.1047e-03
 b1hi_sos =
    1.0000  -2.0000   1.0000   1.0000  -1.9038   0.9082
 g1hi = 0.9530
 
 ===========================================
 
 b2lo_sos =
    1.0000   2.0000   1.0000   1.0000  -0.9428   0.3333
 g2lo = 0.097631
 b2hi_sos =
    1.0000  -2.0000   1.0000   1.0000  -0.9428   0.3333
 g2hi = 0.5690
 
 ===========================================

 b3lo_sos =
    1.0000   2.0000   1.0000   1.0000  -1.9630   0.9637
 g3lo = 1.6822e-04
 b3hi_sos =
    1.0000  -2.0000   1.0000   1.0000  -1.9630   0.9637
 g3hi = 0.9817

*/

Crossover_5_band::Crossover_5_band(uint32_t bufsize) {
	m_bufsize = bufsize;
	
	
    /* S0, f0 (1.8 kHz) */
    s0loL = new float[m_bufsize];
    s0loR = new float[m_bufsize];
    s0hiL = new float[m_bufsize];
    s0hiR = new float[m_bufsize];
    
    /* S1, f1 (520 Hz)*/

    s1loL = new float[m_bufsize];
	s1loR = new float[m_bufsize];
    s1hiL = new float[m_bufsize];
    s1hiR = new float[m_bufsize];

    /*
    b0lo_sos =
       1.0000   2.0000   1.0000   1.0000  -1.6692   0.7166
    g0lo = 0.011858
    b0hi_sos =
       1.0000  -2.0000   1.0000   1.0000  -1.6692   0.7166
    g0hi = 0.8465
    
    
    b0lo_sos = new SOS(0.011858, 1.0000,   2.0000,   1.0000,   1.0000,  -1.6692,   0.7166);
    b0hi_sos = new SOS(0.8465, 1.0000,  -2.0000,   1.0000,   1.0000,  -1.6692,   0.7166);
    
    
    b1lo_sos =
       1.0000   2.0000   1.0000   1.0000  -1.9038   0.9082
    g1lo = 1.1047e-03
    b1hi_sos =
       1.0000  -2.0000   1.0000   1.0000  -1.9038   0.9082
    g1hi = 0.9530
   
    
    b1lo_sos = new SOS(1.1047e-03, 1.0000,   2.0000,   1.0000,   1.0000,  -1.9038,   0.9082);
    b1hi_sos = new SOS(0.9530, 1.0000,  -2.0000,   1.0000,   1.0000,  -1.9038,   0.9082);
    
    
    b2lo_sos =
       1.0000   2.0000   1.0000   1.0000  -0.9428   0.3333
    g2lo = 0.097631
    b2hi_sos =
       1.0000  -2.0000   1.0000   1.0000  -0.9428   0.3333
    g2hi = 0.5690
    
    
    b2lo_sos = new SOS(0.097631, 1.0000,   2.0000,   1.0000,   1.0000,  -0.9428,   0.3333);
    b2hi_sos = new SOS(0.5690, 1.0000,  -2.0000,   1.0000,   1.0000,  -0.9428,   0.3333);
    
     
    b3lo_sos =
       1.0000   2.0000   1.0000   1.0000  -1.9630   0.9637
    g3lo = 1.6822e-04
    b3hi_sos =
       1.0000  -2.0000   1.0000   1.0000  -1.9630   0.9637
    g3hi = 0.9817
    
    
    b3lo_sos = new SOS(1.6822e-04, 1.0000,   2.0000,   1.0000,   1.0000,  -1.9630,   0.9637);
    b3hi_sos = new SOS(0.9817, 1.0000,  -2.0000,   1.0000,   1.0000,  -1.9630,   0.9637);
    */

	f0loL = new tonekids::dsp::FilterLR4(48000.0f, 1800.0f, tonekids::dsp::BUTTERWORTH_TYPE::kLoPass, m_bufsize);
	f0loR = new tonekids::dsp::FilterLR4(48000.0f, 1800.0f, tonekids::dsp::BUTTERWORTH_TYPE::kLoPass, m_bufsize);
	f0hiL = new tonekids::dsp::FilterLR4(48000.0f, 1800.0f, tonekids::dsp::BUTTERWORTH_TYPE::kHiPass, m_bufsize);
	f0hiR = new tonekids::dsp::FilterLR4(48000.0f, 1800.0f, tonekids::dsp::BUTTERWORTH_TYPE::kHiPass, m_bufsize);
	f1loL = new tonekids::dsp::FilterLR4(48000.0f, 520.0f, tonekids::dsp::BUTTERWORTH_TYPE::kLoPass, m_bufsize);
	f1loR = new tonekids::dsp::FilterLR4(48000.0f, 520.0f, tonekids::dsp::BUTTERWORTH_TYPE::kLoPass, m_bufsize);
	f1hiL = new tonekids::dsp::FilterLR4(48000.0f, 520.0f, tonekids::dsp::BUTTERWORTH_TYPE::kHiPass, m_bufsize);
	f1hiR = new tonekids::dsp::FilterLR4(48000.0f, 520.0f, tonekids::dsp::BUTTERWORTH_TYPE::kHiPass, m_bufsize);
	f2loL = new tonekids::dsp::FilterLR4(48000.0f, 6000.0f, tonekids::dsp::BUTTERWORTH_TYPE::kLoPass, m_bufsize);
	f2loR = new tonekids::dsp::FilterLR4(48000.0f, 6000.0f, tonekids::dsp::BUTTERWORTH_TYPE::kLoPass, m_bufsize);
	f2hiL = new tonekids::dsp::FilterLR4(48000.0f, 6000.0f, tonekids::dsp::BUTTERWORTH_TYPE::kHiPass, m_bufsize);
	f2hiR = new tonekids::dsp::FilterLR4(48000.0f, 6000.0f, tonekids::dsp::BUTTERWORTH_TYPE::kHiPass, m_bufsize);
	f3loL = new tonekids::dsp::FilterLR4(48000.0f, 200.0f, tonekids::dsp::BUTTERWORTH_TYPE::kLoPass, m_bufsize);
	f3loR = new tonekids::dsp::FilterLR4(48000.0f, 200.0f, tonekids::dsp::BUTTERWORTH_TYPE::kLoPass, m_bufsize);
	f3hiL = new tonekids::dsp::FilterLR4(48000.0f, 200.0f, tonekids::dsp::BUTTERWORTH_TYPE::kHiPass, m_bufsize);
	f3hiR = new tonekids::dsp::FilterLR4(48000.0f, 200.0f, tonekids::dsp::BUTTERWORTH_TYPE::kHiPass, m_bufsize);
    
    s0f1apL = new tonekids::dsp::AllpassFilterLR4(48000.0f, 520.0f, m_bufsize);
    s0f1apR = new tonekids::dsp::AllpassFilterLR4(48000.0f, 520.0f, m_bufsize);
    s0f2apL = new tonekids::dsp::AllpassFilterLR4(48000.0f, 6000.0f, m_bufsize);
    s0f2apR = new tonekids::dsp::AllpassFilterLR4(48000.0f, 6000.0f, m_bufsize);
    s0f3apL = new tonekids::dsp::AllpassFilterLR4(48000.0f, 200.0f, m_bufsize);
    s0f3apR = new tonekids::dsp::AllpassFilterLR4(48000.0f, 200.0f, m_bufsize);
    s1f3apL = new tonekids::dsp::AllpassFilterLR4(48000.0f, 200.0f, m_bufsize);
    s1f3apR = new tonekids::dsp::AllpassFilterLR4(48000.0f, 200.0f, m_bufsize);
}

Crossover_5_band::~Crossover_5_band() {
	delete[] s0loL;
	delete[] s0loR;
	delete[] s0hiL;
	delete[] s0hiR;
	delete[] s1loL;
	delete[] s1loR;
    delete[] s1hiL;
    delete[] s1hiR;

	delete f0loL;
	delete f0loR;
	delete f0hiL;
	delete f0hiR;
	delete f1loL;
	delete f1loR;
	delete f1hiL;
	delete f1hiR;
	delete f2loL;
	delete f2loR;
	delete f2hiL;
	delete f2hiR;
	delete f3loL;
	delete f3loR;
	delete f3hiL;
	delete f3hiR;

    delete s0f1apL;
    delete s0f1apR;
    delete s0f2apL;
    delete s0f2apR;
    delete s0f3apL;
    delete s0f3apR;
    delete s1f3apL;
    delete s1f3apR;
}

void Crossover_5_band::band_enable(bool _b0, bool _b1, bool _b2, bool _b3, bool _b4)
{
	be0 = _b0;
	be1 = _b1;
	be2 = _b2;
	be3 = _b3;
	be4 = _b4;
}

/* Outs must be a float**, which is allocated by the caller, and is
 * a 2xn array of float vals corresponding to the L/R channels */
void Crossover_5_band::process(float* inL, float* inR, float** outb1, float** outb2, float** outb3,
		                     float** outb4, float** outb5, int samps)
{
	band0outL = outb1[0];
	band0outR = outb1[1];
	band1outL = outb2[0];
	band1outR = outb2[1];
	band2outL = outb3[0];
	band2outR = outb3[1];
	band3outL = outb4[0];
	band3outR = outb4[1];
	band4outL = outb5[0];
	band4outR = outb5[1];

	/* OK, we're gonna look at this from a standpoint of processing stages.
	 *
	 * S0LO:  f0LP, f2AP
	 * S0HI:  f0HP, f1AP, f3AP
	 *
	 * S2LO: f2LP  ---BAND 3 OUTPUT
     * S2HI: f2HP  ---BAND 4 OUTPUT
     *
     * S1LO: f1LP
     * S1HI: f1HP, f3AP  ---BAND 2 OUTPUT
     *
     * S3LO: f3LP  ---BAND 0 OUTPUT
     * S3HI: f3HP  ---BAND 1 OUTPUT
	 */

    /* S0, f0 (1.8 kHz)
     * S0LO:  f0LP, f2AP
     * S0HI:  f0HP, f1AP, f3AP
     */

	/* Low-pass */
	f0loL->process(inL, s0loL);
	f0loR->process(inR, s0loR);
	s0f2apL->process(s0loL, s0loL);
	s0f2apR->process(s0loR, s0loR);

	/* High-pass */
	f0hiL->process(inL, s0hiL);
	f0hiR->process(inR, s0hiR);
	s0f1apL->process(s0hiL, s0hiL);
	s0f1apR->process(s0hiR, s0hiR);
	s0f3apL->process(s0hiL, s0hiL);
	s0f3apR->process(s0hiR, s0hiR);

    /* S2, f2 (6 kHz)
    * S2LO: f2LP  ---BAND 3 OUTPUT
    * S2HI: f2HP  ---BAND 4 OUTPUT
    */
     
	/* Low-pass */
	if(true == be3) {
		f2loL->process(s0hiL, band3outL);
		f2loR->process(s0hiR, band3outR);
	}
    else {
        memset(band3outL, 0, samps*sizeof(float));
        memset(band3outR, 0, samps*sizeof(float));
    }

	/* High-pass */
	if(true == be4)	{
		f2hiL->process(s0hiL, band4outL);
		f2hiR->process(s0hiR, band4outR);
	}
    else {
        memset(band4outL, 0, samps*sizeof(float));
        memset(band4outR, 0, samps*sizeof(float));
    }

	/* S1, f1 (520 Hz)
     * S1LO: f1LP
     * S1HI: f1HP, f3AP  ---BAND 2 OUTPUT
     */

	/* Low-pass */
	f1loL->process(s0loL, s1loL);
	f1loR->process(s0loR, s1loR);

	/* High-pass */
    
    f1hiL->process(s0loL, s1hiL);
    f1hiR->process(s0loL, s1hiR);
    if(true == be2) {
        s1f3apL->process(s1hiL, band2outL);
        s1f3apR->process(s1hiR, band2outR);
    }
    else {
        memset(band2outL, 0, samps*sizeof(float));
        memset(band2outR, 0, samps*sizeof(float));
    }

    /* S3, f3 (200 Hz)
     * S3LO: f3LP   --- BAND 0 OUTPUT
     * S1HI: f3HP   ---BAND 1 OUTPUT
     */
	if(true == be0)	{
		f3loL->process(s1loL, band0outL);
		f3loR->process(s1loR, band0outR);
	}
    else {
        memset(band0outL, 0, samps*sizeof(float));
        memset(band0outR, 0, samps*sizeof(float));
    }
    
	if(true == be1)	{
		f3hiL->process(s1loL, band1outL);
		f3hiR->process(s1loR, band1outR);
	}
    else {
        memset(band1outL, 0, samps*sizeof(float));
        memset(band1outR, 0, samps*sizeof(float));
    }
}

