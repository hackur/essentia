/*
 * Copyright (C) 2006-2008 Music Technology Group (MTG)
 *                         Universitat Pompeu Fabra
 *
 */

/*
 * This file is a port of the file pitchyinfft.h from aubio,
 * http://aubio.piem.org/, in its version 0.3.2.
 *
 * The port was written by the author of aubio, Paul Brossier
 * <piem@altern.org>.
 */

#ifndef ESSENTIA_PITCHYINFFT_H
#define ESSENTIA_PITCHYINFFT_H

#include "algorithmfactory.h"

namespace essentia {
namespace standard {

class PitchYinFFT : public Algorithm {

 private:
  Input<std::vector<Real> > _spectrum;
  Output<Real> _pitch;
  Output<Real> _pitchConfidence;

  Algorithm* _fft;
  Algorithm* _cart2polar;

  std::vector<Real> _resPhase; /** complex vector to compute square difference function */
  std::vector<Real> _resNorm;
  std::vector<Real> _sqrMag;   /** square difference function */
  std::vector<Real> _weight;   /** spectral weighting window (psychoacoustic model) */
  std::vector<Real> _yin;      /** Yin function */
  Real _sampleRate;            /** sampling rate of the audio signal */
  int _frameSize;
  //Real _tolerance;
  int _tauMin;
  int _tauMax;


 public:
  PitchYinFFT() {
    declareInput(_spectrum, "spectrum", "the input spectrum (preferably created with a hann window)");
    declareOutput(_pitch, "pitch", "detected pitch [Hz]");
    declareOutput(_pitchConfidence, "pitchConfidence", "confidence with which the pitch was detected [0,1]");

    _fft = AlgorithmFactory::create("FFT");
    _cart2polar = AlgorithmFactory::create("CartesianToPolar");
  }

  ~PitchYinFFT() {
    delete _fft;
    delete _cart2polar;
  };

  void declareParameters() {
    declareParameter("frameSize", "number of samples in the input spectrum", "[2,inf)", 2048);
    declareParameter("sampleRate", "sampling rate of the input spectrum [Hz]", "(0,inf)", 44100.);
    declareParameter("minFrequency", "the minimum allowed frequency [Hz]", "(0,inf)", 20.0);
    declareParameter("maxFrequency", "the maximum allowed frequency [Hz]", "(0,inf)", 22050.0);
    //declareParameter("tolerance", "tolerance for peak detection", "[0,1]", 0.75);
  }

  void configure();
  void compute();

  void spectralWeights();

  static const char* name;
  static const char* description;

}; // class PitchYinFFT

} // namespace standard
} // namespace essentia


#include "streamingalgorithmwrapper.h"

namespace essentia {
namespace streaming {

class PitchYinFFT : public StreamingAlgorithmWrapper {

 protected:
  Sink<std::vector<Real> > _spectrum;
  Source<Real> _pitch;
  Source<Real> _pitchConfidence;

 public:
  PitchYinFFT() {
    declareAlgorithm("PitchYinFFT");
    declareInput(_spectrum, TOKEN, "spectrum");
    declareOutput(_pitch, TOKEN, "pitch");
    declareOutput(_pitchConfidence, TOKEN, "pitchConfidence");
  }
};

} // namespace streaming
} // namespace essentia

#endif // ESSENTIA_PITCHYINFFT_H
