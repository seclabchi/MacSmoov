/*
 * Crossover_5_band.h
 *
 *  Created on: Aug 26, 2023
 *      Author: zaremba
 */

#ifndef INCLUDE_Crossover_5_band_H_
#define INCLUDE_Crossover_5_band_H_

#include <stdint.h>
#include "filter_lr4.h"

class Crossover_5_band {
public:
    Crossover_5_band(uint32_t bufsize);
	virtual ~Crossover_5_band();
	void process(float* inL, float* inR, float** outb1, float** outb2, float** outb3,
			     float** outb4, float** outb5, int samps);
	void band_enable(bool _b0, bool _b1, bool _b2, bool _b3, bool _b4);

private:

	uint32_t m_bufsize;
	float* band0outL, *band0outR, *band1outL, *band1outR, *band2outL, *band2outR;
	float* band3outL, *band3outR, *band4outL, *band4outR;
	float* s0loL, *s0loR, *s0hiL, *s0hiR, *s1loL, *s1loR, *s1hiL, *s1hiR;

	bool be0, be1, be2, be3, be4;

    SOS* b0lo_sos;
    SOS* b0hi_sos;
    SOS* b1lo_sos;
    SOS* b1hi_sos;
    SOS* b2lo_sos;
    SOS* b2hi_sos;
    SOS* b3lo_sos;
    SOS* b3hi_sos;

	FilterLR4 *f0loL, *f0loR, *f0hiL, *f0hiR;
    FilterLR4 *f1loL, *f1loR, *f1hiL, *f1hiR;
    FilterLR4 *f2loL, *f2loR, *f2hiL, *f2hiR;
    FilterLR4 *f3loL, *f3loR, *f3hiL, *f3hiR;

    AllpassFilterLR4* s0f1apL, *s0f1apR, *s0f2apL, *s0f2apR, *s0f3apL, *s0f3apR;
    AllpassFilterLR4* s1f3apL, *s1f3apR;
};

#endif /* INCLUDE_Crossover_5_band_H_ */
