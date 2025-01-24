#ifndef __BUTTERWORTH_H__
#define __BUTTERWORTH_H__

#include <complex>
#include <vector>
#include "biquad.h"

namespace tonekids {
namespace dsp {

typedef std::complex <double> complex_double;

typedef enum {
    kLoPass     = 10000,
    kHiPass     = 10001,
    kBandPass   = 10002,
    kBandStop   = 10003,
    kLoShelf    = 10004,
    kHiShelf    = 10005, // high order EQ
    kParametric = 10006  // high order EQ
} BUTTERWORTH_TYPE;

class Butterworth {
    public:
        Butterworth();
        virtual ~Butterworth();

        static std::vector <complex_double> prototypeAnalogLowPass(int filterOrder);

        bool loPass(double fs, double f1, double f2, int filterOrder, std::vector <Biquad> & coeffs, double & overallGain) {
            return coefficients(kLoPass, fs, f1, f2, filterOrder, coeffs, overallGain);
        }

        bool hiPass(double fs, double f1, double f2, int filterOrder, std::vector <Biquad> & coeffs, double & overallGain) {
            return coefficients(kHiPass, fs, f1, f2, filterOrder, coeffs, overallGain);
        }

        // generic coeffs
        bool coefficients(BUTTERWORTH_TYPE filter, const double fs, const double freq1_cutoff, const double freq2_cutoff, const int filterOrder,
                      std::vector <Biquad> & coeffs, double & overallGain);

        // high order EQ
        bool coefficientsEQ(BUTTERWORTH_TYPE filter, const double fs, const double f1, const double f2, const int filterOrder,
                      std::vector <Biquad> & coeffs, double overallGain);

    private:
        double blt(complex_double & sz);
        bool s2Z();
        bool zp2SOS();

        // Private helper methods to convert the lowpass analog prototype to the desired filter type
        void convert2lopass();
        void convert2hipass();
        void convert2bandpass();
        void convert2bandstop();

        // Internal state used during computation of coefficients
        double f1, f2;

        int numPoles, numZeros;
        std::vector <complex_double> zeros;
        std::vector <complex_double> poles;

        double Wc; // Omega cutoff == passband edge freq
        double bw; // Bandwidth

        double gain;
        double preBLTgain;

        int nba;
        double * ba;
};

} /* namespace dsp */
} /* namespace tonekids */

#endif /* __BUTTERWORTH_H__ */
