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
#include <algorithm>
#include <complex>


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

	// opts_.num_feats;
	// opts_.wavelet_type;
	// opts_.decomposition_level;

	if(opts_.wavelet_type == "haar"){

		std::vector<BaseFloat> output, avg, diff;

		while(iteration > 0){

			//KALDI_LOG << "444ghost.LOG in feature-wavelet.cc: iteration = " << iteration;

			BaseFloat max = 0;
			BaseFloat min = 0;
			BaseFloat norm = 0;

			if(iteration == opts_.decomposition_level){

				//KALDI_LOG << "444ghost.LOG in feature-wavelet.cc: first-band length = " << length;
				for(int i = 0; i < length / 2; i++){

					avg.push_back(((*signal_frame)(2 * i) + (*signal_frame)((2 * i) + 1)) / 2); 
					diff.push_back(((*signal_frame)(2 * i) - (*signal_frame)((2 * i) + 1)) / 2);
					norm += pow((*signal_frame)(2 * i), 2) + pow((*signal_frame)((2 * i) + 1), 2);
				}
			} else{

				//KALDI_LOG << "444ghost.LOG in feature-wavelet.cc: subband length = " << length;

				std::vector<BaseFloat> subband;
				subband = avg;

				avg.clear();
				diff.clear();

				for(int i = 0; i < length / 2; i++){

					avg.push_back(((subband.at(2 * i) + subband.at((2 * i) + 1)) / 2)); 
					diff.push_back(((subband.at(2 * i) - subband.at((2 * i) + 1)) / 2));
					norm += pow(subband.at(2 * i), 2) + pow(subband.at((2 * i) + 1), 2); 	
				}
			}

			max = *std::max_element(diff.begin(), diff.end());
			min = *std::min_element(diff.begin(), diff.end());
			
			//KALDI_LOG << "444ghost.LOG in feature-wavelet.cc: max = " << max;
			//KALDI_LOG << "444ghost.LOG in feature-wavelet.cc: min = " << min;
			//KALDI_LOG << "444ghost.LOG in feature-wavelet.cc: norm = " << norm;

			output.insert(output.end(), max);
			output.insert(output.end(), min);
			output.insert(output.end(), sqrt(norm)); // L2 norm, square.

			length = length / 2;

			iteration--;

			if(iteration == 0){

				max = *std::max_element(avg.begin(), avg.end());
				min = *std::min_element(avg.begin(), avg.end());
				
				//KALDI_LOG << "444ghost.LOG in feature-wavelet.cc: max = " << max;
				//KALDI_LOG << "444ghost.LOG in feature-wavelet.cc: min = " << min;
				//KALDI_LOG << "444ghost.LOG in feature-wavelet.cc: norm = " << norm;

				output.insert(output.end(), max);
				output.insert(output.end(), min);
				output.insert(output.end(), sqrt(norm));
			}

			//KALDI_LOG << "444ghost.LOG in feature-wavelet.cc: output.size() = " << output.size();
		}

		for(int i = 0; i < opts_.num_feats; i++){

			(*feature)(i) = output.at(i);
			KALDI_LOG << "444ghost.LOG in feature-wavelet.cc: (*feature)(" << i << ") = " << output.at(i);
		}
	}
	//KALDI_LOG << "---------------------------------";
}

WaveletComputer::WaveletComputer(const WaveletOptions &opts):
	opts_(opts){

	KALDI_LOG << "444ghost.LOG in feature-wavelet.cc: num_feats = " << opts.num_feats;
	KALDI_LOG << "444ghost.LOG in feature-wavelet.cc: wavelet_type = "<< opts.wavelet_type;
	KALDI_LOG << "444ghost.LOG in feature-wavelet.cc: decomposition_level = " << opts.decomposition_level;
}

WaveletComputer::~WaveletComputer() {
 
}

}  // namespace kaldi
