/*
 * Copyright (C) 2006-2013  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of Essentia
 *
 * Essentia is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 */

#ifndef ESSENTIA_VIBRATO_H
#define ESSENTIA_VIBRATO_H

#include "algorithmfactory.h"

namespace essentia {
namespace standard {

class Vibrato : public Algorithm {

 private:
  Input<std::vector<Real> > _pitch;
  Input<Real> _sampleRate;
  Output<std::vector<Real> > _vibFrequency;
  Output<std::vector<Real> > _vibExtend;
    
  Algorithm* frameCutter;
  Algorithm* window;
  Algorithm* spectrum;
  Algorithm* spectralPeaks;

 public:
  Vibrato() {
    declareInput(_pitch, "pitch", "the pitch trajectory [Hz].");
    declareInput(_sampleRate, "sampleRate", "the sample rate of the input pitch contour.");
    declareOutput(_vibFrequency, "vibratoFrequency", "estimated vibrato frquency [Hz]; zero if no vibrato was detected.");
    declareOutput(_vibExtend, "vibratoExtend","estimated vibrato frquency [Hz]; zero if no vibrato was detected.") ;
      
    frameCutter = AlgorithmFactory::create("FrameCutter");
    window = AlgorithmFactory::create("Windowing");
    spectrum = AlgorithmFactory::create("Spectrum");
    spectralPeaks = AlgorithmFactory::create("SpectralPeaks");
  }

  void declareParameters() {
    declareParameter("minFrequency", "minimum considered vibrato frequency [Hz]", "(0,inf)", 4.0);
    declareParameter("maxFrequency", "maximum considered vibrato frequency [Hz]", "(0,inf)", 8.0);
    declareParameter("minExtend", "minimum considered vibrato extend [cents]", "(0,inf)", 50.0);
    declareParameter("maxExtend", "maximum considered vibrato extend [cents]", "(0,inf)", 250.0);
  }

  void compute();
  void configure();

  static const char* name;
  static const char* description;

 protected:
  Real _maxFrequency;
  Real _minFrequency;
  Real _maxExtend;
  Real _minExtend;
  
  int frameSize;
  int fftSize;
  Real sampleRate;
  
};

} // namespace standard
} // namespace essentia

#include "streamingalgorithmwrapper.h"

namespace essentia {
namespace streaming {

class Vibrato : public StreamingAlgorithmWrapper {

 protected:
  Sink<std::vector<Real> > _pitch;
  Sink<Real> _sampleRate;
  Source<std::vector<Real> > _vibFrequency;
  Source<std::vector<Real> > _vibExtend;

 public:
  Vibrato() {
    declareAlgorithm("Vibrato");
    declareInput(_pitch, TOKEN, "pitch");
    declareInput(_sampleRate, TOKEN, "sampleRate");
    declareOutput(_vibFrequency, TOKEN, "vibratoFrequency");
    declareOutput(_vibExtend, TOKEN, "vibratoExtend");
  }
};

} // namespace streaming
} // namespace essentia

#endif // ESSENTIA_VIBRATO_H
