#ifndef __BIQUAD_H__
#define __BIQUAD_H__

#include <cstdint>

/*
 * This biquad module computes in Direct Form II Transposed, and the coeffs
 * passed into it will be sign-flipped for the "a" terms.
 */

namespace tonekids {
namespace dsp {

typedef struct {
    float b0;
    float b1;
    float b2;
    float b3;
    float b4;
    float a0;
    float a1;
    float a2;
    float a3;
    float a4;
} SOS;

class Biquad {
    public:
        Biquad();
        Biquad(const SOS& _coeffs, float _gain);
        virtual ~Biquad();
        void process(float* in, float* out, uint32_t nSamp);

        // Coefficients for a DF2T fourth order section (Used for EQ filters)
        void DF2TFourthOrderSection(double B0, double B1, double B2, double B3, double B4,
                                double A0, double A1, double A2, double A3, double A4, double _gain);

        // Coefficients for a DF2T biquad section.
        void DF2TBiquad(double B0, double B1, double B2,
                        double A0, double A1, double A2, double _gain);
    private:
        void initialize();
        SOS coeffs;  // SOS coeffs
        float gain;
        float z1;
        float z2;
};

} /* dsp */
} /* tonekids */

#endif /* __BIQUAD_H__ */
