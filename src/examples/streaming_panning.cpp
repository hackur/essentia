#include <iostream>
#include "algorithmfactory.h"
#include "fileoutput.h"
#include "network.h"
#include "poolstorage.h"

using namespace essentia;
using namespace essentia::streaming;
using namespace essentia::scheduler;
using namespace std;

int main(int argc, char** argv) {
  if (argc < 2) {
      cout << "Error: wrong number of arguments" << endl;
      cout << "Usage: " << argv[0] << " input_audiofile" << endl;
      exit(1);
  }

  // parameters:
  int framesize = 8192;
  int hopsize = 2048;
  int zp = 1;
  Real sr = 44100.;

  // register the algorithms in the factory(ies)
  essentia::init();

  streaming::AlgorithmFactory& factory = streaming::AlgorithmFactory::instance();

  Algorithm* loader     = factory.create("AudioLoader",
                                         "filename", argv[1]);

  Algorithm* demuxer    = factory.create("StereoDemuxer");

  Algorithm* fc_left    = factory.create("FrameCutter",
                                         "frameSize", framesize,
                                         "hopSize", hopsize,
                                         "startFromZero", false,
                                         "silentFrames", "noise");

  Algorithm* fc_right   = factory.create("FrameCutter",
                                         "frameSize", framesize,
                                         "hopSize", hopsize,
                                         "startFromZero", false,
                                         "silentFrames", "noise");

  Algorithm* w_left     = factory.create("Windowing",
                                         "size", framesize,
                                         "zeroPadding", framesize*zp,
                                         "type", "hann");

  Algorithm* w_right    = factory.create("Windowing",
                                         "size", framesize,
                                         "zeroPadding", framesize*zp,
                                         "type", "hann");

  Algorithm* spec_left  = factory.create("Spectrum",
                                         "size", framesize*zp);

  Algorithm* spec_right = factory.create("Spectrum",
                                         "size", framesize*zp);

  Algorithm* pan        = factory.create("Panning",
                                         "sampleRate", sr,
                                         "averageFrames", 43, // 2 seconds * sr/hopsize
                                         "panningBins", 512,
                                         "numCoeffs", 20,
                                         "numBands", 1,
                                         "warpedPanorama", true);

  Algorithm* file = new FileOutput<TNT::Array2D<Real> >();
  file->configure("filename", "panning_coeffs.txt",
                  "mode", "text");

  Pool pool;
  // audio
  loader->output("audio")           >>  demuxer->input("audio");
  loader->output("sampleRate")      >>  NOWHERE;
  loader->output("numberChannels")  >>  NOWHERE;

  // demuxer
  demuxer->output("left")           >>  fc_left->input("signal");
  demuxer->output("right")          >>  fc_right->input("signal");

  // left channel
  fc_left->output("frame")          >>  w_left->input("frame");
  w_left->output("frame")           >>  spec_left->input("frame");
  spec_left->output("spectrum")     >>  pan->input("spectrumLeft");
  // right channel
  fc_right->output("frame")         >>  w_right->input("frame");
  w_right->output("frame")          >>  spec_right->input("frame");
  spec_right->output("spectrum")    >>  pan->input("spectrumRight");

  // panning:
  pan->output("panningCoeffs")      >>  file->input("data");
  pan->output("panningCoeffs")      >>  PC(pool, "lowlevel.panning_coefficients");


  Network network(loader);
  network.run();

  standard::Algorithm* output = standard::AlgorithmFactory::create("YamlOutput",
                                                                   "filename", "panning_coeffs.yaml");
  output->input("pool").set(pool);
  output->compute();
  delete output;


  essentia::shutdown();

  cout << "Results written in panning_coeffs.txt and panning_coeffs.yaml" << endl;

  return 0;
}
