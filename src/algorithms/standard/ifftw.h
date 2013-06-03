/*
 * Copyright (C) 2006-2008 Music Technology Group (MTG)
 *                         Universitat Pompeu Fabra
 *
 */

#ifndef ESSENTIA_IFFTW_H
#define ESSENTIA_IFFTW_H

#include "algorithm.h"
#include "threading.h"
#include <complex>
#include <fftw3.h>

namespace essentia {
namespace standard {

class IFFTW : public Algorithm {

 protected:
  Input<std::vector<std::complex<Real> > > _fft;
  Output<std::vector<Real> > _signal;

 public:
  IFFTW() : _fftPlan(0), _input(0), _output(0) {
    declareInput(_fft, "fft", "the input frame");
    declareOutput(_signal, "frame", "the IFFT of the input frame");
  }

  ~IFFTW();

  void declareParameters() {
    declareParameter("size", "the expected size of the input frame. This is purely optional and only targeted at optimizing the creation time of the FFT object", "[1,inf)", 1024);
  }


  void compute();
  void configure();

  static const char* name;
  static const char* description;

 protected:
  fftwf_plan _fftPlan;
  int _fftPlanSize;
  std::complex<Real>* _input;
  Real* _output;

  void createFFTObject(int size);
};

} // namespace standard
} // namespace essentia

#include "streamingalgorithmwrapper.h"

namespace essentia {
namespace streaming {

class IFFTW : public StreamingAlgorithmWrapper {

 protected:
  Sink<std::vector<std::complex<Real> > > _fft;
  Source<std::vector<Real> > _signal;

 public:
  IFFTW() {
    declareAlgorithm("IFFT");
    declareInput(_fft, TOKEN, "fft");
    declareOutput(_signal, TOKEN, "frame");
  }
};

} // namespace streaming
} // namespace essentia

#endif // ESSENTIA_IFFTW_H
