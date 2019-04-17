// feat/feature-mfcc.h

// Copyright 2009-2011  Karel Vesely;  Petr Motlicek;  Saarland University
//           2014-2016  Johns Hopkins University (author: Daniel Povey)

// See ../../COPYING for clarification regarding multiple authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED
// WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE,
// MERCHANTABLITY OR NON-INFRINGEMENT.
// See the Apache 2 License for the specific language governing permissions and
// limitations under the License.

#include <map>
#include <string>

#include "feat/feature-common.h"
#include "feat/feature-functions.h"
#include "feat/feature-window.h"
#include "feat/mel-computations.h"

namespace kaldi {
/// @addtogroup  feat FeatureExtraction
/// @{

// This is the new-style interface to the Wavelet computation.
class WaveletComputer {
 public:
  typedef WaveletOptions Options;
  explicit WaveletComputer(const WaveletOptions &opts);
  WaveletComputer(const WaveletComputer &other);

  const FrameExtractionOptions &GetFrameOptions() const {
    return opts_.frame_opts;
  }

  int32 Dim() const { return opts_.num_feats; }

  void Compute(VectorBase<BaseFloat> *signal_frame,
               VectorBase<BaseFloat> *feature);
};

typedef OfflineFeatureTpl<WaveletComputer> Wavelet;

/// @} End of "addtogroup feat"
}  // namespace kaldi

