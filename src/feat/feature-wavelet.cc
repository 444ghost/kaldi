// feat/feature-wavelet.cc

// Copyright 2019- Euisung Kim

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
#include <algorithm>
#include <complex>


namespace kaldi {

void DWT(VectorBase<BaseFloat> *signal, int length, int J, std::vector<BaseFloat> *output, std::string wavelet_type){

	if(signal->Dim() < pow(2, J)){

		KALDI_ERR << "444ghost.ERROR in feature-wavelet.cc: too many decomposition levels for current window size(sample size)";
		return;
	}

	if(wavelet_type == "haar"){

		if(J == 0){

			output->insert(output->end(), signal->Max());
			output->insert(output->end(), signal->Min());
			output->insert(output->end(), signal->Norm(2));
			return;
		}

		Vector<BaseFloat> vAvg(length/2), vDiff(length/2);

		for(int i = 0; i < length/2; i++){

			vAvg(i) = ((*signal)(2*i) + (*signal)((2*i)+1)) / 2;
			vDiff(i) = ((*signal)(2*i) - (*signal)((2*i)+1)) / 2;
		}

		output->insert(output->end(), vDiff.Max());
		output->insert(output->end(), vDiff.Min());
		output->insert(output->end(), vDiff.Norm(2));

		DWT(&vAvg, length/2, --J, output, wavelet_type);
	
	}

	return;

	/*
	for(int i = 0; i < signal->Dim(); i++){

		KALDI_LOG << "signal(" << i << ") = " << (*signal)(i);
	}
	
	for(int i = 0; i < length/2; i++){

		KALDI_LOG << "vAvg(" << i << ") = " << vAvg(i);
	}

	for(int i = 0; i < length/2; i++){

		KALDI_LOG << "vDiff(" << i << ") = " << vDiff(i);
	}
	*/
}

void WPT(VectorBase<BaseFloat> *signal, int length, int J, int J2, std::vector<BaseFloat> *output, std::string wavelet_type){

	KALDI_LOG << "signal->Dim() = " << signal->Dim();
	KALDI_LOG << "J = " << J;
	KALDI_LOG << "J2 = " << J2;

	if(wavelet_type == "haar"){

		if(J == 0 || J2 == 0){

			output->insert(output->end(), signal->Max());
			output->insert(output->end(), signal->Min());
			output->insert(output->end(), signal->Norm(2));
			KALDI_LOG << "exit here?" << J;
			return;
		}

		Vector<BaseFloat> vAvg(length/2), vDiff(length/2);

		for(int i = 0; i < length/2; i++){

			vAvg(i) = ((*signal)(2*i) + (*signal)((2*i)+1)) / 2;
			vDiff(i) = ((*signal)(2*i) - (*signal)((2*i)+1)) / 2;
		}

		output->insert(output->end(), vAvg.Max());
		output->insert(output->end(), vAvg.Min());
		output->insert(output->end(), vAvg.Norm(2));
		output->insert(output->end(), vDiff.Max());
		output->insert(output->end(), vDiff.Min());
		output->insert(output->end(), vDiff.Norm(2));

		for(int i = 0; i < length/2; i++){

			KALDI_LOG << "vAvg(" << i << ") = " << vAvg(i);
		}

		for(int i = 0; i < length/2; i++){

			KALDI_LOG << "vDiff(" << i << ") = " << vDiff(i);
		}

		KALDI_LOG << "**************************************";

		WPT(&vAvg, length/2, --J, J2, output, wavelet_type);
		WPT(&vDiff, length/2, J, --J2, output, wavelet_type);
	
	}

	return;
}

void WaveletComputer::Compute(VectorBase<BaseFloat> *signal_frame,
                              VectorBase<BaseFloat> *feature) {
	KALDI_ASSERT(
			signal_frame->Dim() ==
		 	opts_.frame_opts.PaddedWindowSize() &&
	                feature->Dim() == this->Dim()
		    );

	std::string transform_type = opts_.transform_type;
	std::string wavelet_type = opts_.wavelet_type;

	if(transform_type == "dwt"){

		int length = signal_frame->Dim();
		int J = opts_.decomposition_level;
		std::vector<BaseFloat> output;
		Vector<BaseFloat> signal(*signal_frame);

		DWT(&signal, length, J, &output, wavelet_type);

		for(int i = 0; i < opts_.num_feats; i++){

			(*feature)(i) = output.at(i);
			KALDI_LOG << "444ghost.LOG in feature-wavelet.cc: (*feature)(" << i << ") = " << (*feature)(i);
		}
	} else if(transform_type == "wpt"){

		int length = signal_frame->Dim();
		int J = opts_.decomposition_level;
		std::vector<BaseFloat> output;
		Vector<BaseFloat> signal(16);

		for(int i = 0; i < 16; i++){
		
			signal(i) = i+1;			
		}

		KALDI_LOG << "signal.Dim() = " << signal.Dim();

		WPT(&signal, 16, 3, 3, &output, wavelet_type);
	}

	// opts_.num_feats;
	// opts_.wavelet_type;
	// opts_.decomposition_level;

	// Downsampling can be implemented by iterating through the signal by every 2 steps. 
	// For example n~n+1 and n+2~n+3 but no n+1~n+2 

	/*			
	Vector<BaseFloat> signal(16);

	for(int i = 0; i < 16; i++){
	
		signal(i) = i+1;			
	}
				

	DWT(&signal, 16, 3, &output);

	for(int i = 0; i < output.size(); i++){

		KALDI_LOG << "output.at(" << i << ") = " << output.at(i);
	}
	*/
}

WaveletComputer::WaveletComputer(const WaveletOptions &opts):
	opts_(opts){

	KALDI_LOG << "444ghost.LOG in feature-wavelet.cc--------------->";
	KALDI_LOG << "round_to_power_of_two = " << opts.frame_opts.round_to_power_of_two;
	KALDI_LOG << "num_feats = " << opts.num_feats;
	KALDI_LOG << "wavelet_type = "<< opts.wavelet_type;
	KALDI_LOG << "decomposition_level = " << opts.decomposition_level;
	KALDI_LOG << "transform_type = " << opts.transform_type;
	KALDI_LOG << "<---------------444ghost.LOG in feature-wavelet.cc";
}

WaveletComputer::~WaveletComputer() {
 
}

}  // namespace kaldi
