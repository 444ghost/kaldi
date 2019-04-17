// featbin/compute-dwt-feats.cc

// Copyright 2009-2012  Microsoft Corporation
//                      Johns Hopkins University (author: Daniel Povey)
//			444ghost

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

#include "base/kaldi-common.h"
#include "util/common-utils.h"
#include "feat/feature-wavelet.h"
#include "feat/wave-reader.h"

int main(int argc, char *argv[]) {
 
	using namespace kaldi;
	ParseOptions po("444ghost.ERROR: Wrong options");
	WaveletOptions wavelet_opts;

	int32 num_feats;

	wavelet_opts.Register(&po);

	//po.Register("num-feats", &num_feats, "po.Register(\"num-feats\"...");

	po.Read(argc, argv);

	KALDI_LOG << "num_feats = " << wavelet_opts.num_feats;
	KALDI_LOG << "wavelet_type = " << wavelet_opts.wavelet_type;
	KALDI_LOG << "decomposition_level = " << wavelet_opts.decomposition_level;

	std::string hey = po.GetArg(1);
	KALDI_LOG << hey;

	Wavelet wavelet(wavelet_opts);

	return -1;
}

