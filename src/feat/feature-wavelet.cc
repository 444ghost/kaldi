// feat/feature-wavelet.cc

// Copyright 2009-2011  Karel Vesely;  Petr Motlicek
//                2016  Johns Hopkins University (author: Daniel Povey)

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


#include "feat/feature-wavelet.h"


namespace kaldi {

void WaveletComputer::Compute(VectorBase<BaseFloat> *signal_frame,
                              VectorBase<BaseFloat> *feature) {
	KALDI_ASSERT(
			signal_frame->Dim() ==
		 	opts_.frame_opts.PaddedWindowSize() &&
	                feature->Dim() == this->Dim()
		    );

	int length = signal_frame->Dim();
	int iteration = opts_.decomposition_level;
	
	//VectorBase<BaseFloat> dwt_vector;
	//dwt_vector.Resize(opts_.num_feats);

	// opts_.num_feats;
	// opts_.wavelet_type;
	// opts_.decomposition_level;

	if(length % 2 == 0){
	
		KALDI_LOG << "444ghost.LOG in feature-wavelet.cc: even";
	} else{

		KALDI_LOG << "444ghost.LOG in feature-wavelet.cc: odd";
	}
	/*
	if(opts_.wavelet_type == "haar"){

		for(int i = 0; i < length; i++){

		
		}
	}
	*/
	

	
	
}

WaveletComputer::WaveletComputer(const WaveletOptions &opts):
	opts_(opts){

	KALDI_LOG << "444ghost: num_feats = " << opts.num_feats;
	KALDI_LOG << "444ghost: wavelet_type = "<< opts.wavelet_type;
	KALDI_LOG << "444ghost: decomposition_level = " << opts.decomposition_level;
}

WaveletComputer::~WaveletComputer() {
 
}

}  // namespace kaldi
