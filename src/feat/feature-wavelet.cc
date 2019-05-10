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

// future work: convert std::vector<BaseFloat> *output to Vector<BaseFloat> output

#include "feat/feature-wavelet.h"
#include <numeric>
#include <algorithm>
#include <complex>


namespace kaldi {

void DWT(VectorBase<BaseFloat> *signal, int J, std::vector<BaseFloat> *output, std::string wavelet_type, int zoom){

	/*
	if(signal->Dim() < pow(2, J)){

		KALDI_ERR << "444ghost.ERROR in feature-wavelet.cc: too many decomposition levels for current window size(sample size)";
		return;
	}
	*/
	if(wavelet_type == "haar"){

		if(J == 0){

			output->insert(output->end(), signal->Max());
			output->insert(output->end(), signal->Min());
			output->insert(output->end(), signal->Norm(2));
			return;
		}

		Vector<BaseFloat> vLow(signal->Dim()/2), vHigh(signal->Dim()/2);

		KALDI_LOG << "signal->Dim()/2 = " << signal->Dim()/2;

		for(int i = 0; i < signal->Dim()/2; i++){

			vLow(i) = ((*signal)(2*i) + (*signal)((2*i)+1)) / 2;
			vHigh(i) = ((*signal)(2*i) - (*signal)((2*i)+1)) / 2;
		}

		KALDI_LOG << "signal->Dim()/2 = " << signal->Dim()/2;

		if(zoom <= 0){

			output->insert(output->end(), vHigh.Max());
			output->insert(output->end(), vHigh.Min());
			output->insert(output->end(), vHigh.Norm(2));
		}

		DWT(&vLow, --J, output, wavelet_type, --zoom);
	} else if(wavelet_type == "db4"){

		if(signal->Dim() > 3){

			float l0 = 0.4829629131445341, l1 = 0.8365163037378077, l2 = 0.2241438680420134, l3 = -0.1294095225512603;
			float h0 = l3, h1 = -l2, h2 = l1, h3 = -l0;
			
			Vector<BaseFloat> vLow(signal->Dim()/2), vHigh(signal->Dim()/2);

			for(int i = 0; i < signal->Dim() - 3; i = i + 2){ // instead of signal->Dim() - 2 for exactly 4 dimension long signal

				float low = 0, high = 0;

				low += (*signal)(i+0)*l0;
				low += (*signal)(i+1)*l1;
				low += (*signal)(i+2)*l2;
				low += (*signal)(i+3)*l3;

				high += (*signal)(i+0)*h0;
				high += (*signal)(i+1)*h1;
				high += (*signal)(i+2)*h2;
				high += (*signal)(i+3)*h3;
				
				vLow(i/2) = low;
				vHigh(i/2) = high;
			}

			float low = 0, high = 0;

			low += (*signal)(signal->Dim() - 2)*l0;
			low += (*signal)(signal->Dim() - 1)*l1;
			low += (*signal)(0)*l2;
			low += (*signal)(1)*l3;

			high += (*signal)(signal->Dim() - 2)*h0;
			high += (*signal)(signal->Dim() - 1)*h1;
			high += (*signal)(0)*h2;
			high += (*signal)(1)*h3;

			vLow((signal->Dim()/2) - 1) = low;
			vHigh((signal->Dim()/2) - 1) = high;

			if(J == 0){

				output->insert(output->end(), vLow.Max());
				//output->insert(output->end(), vLow.Min());
				output->insert(output->end(), vLow.Norm(2)/vLow.Dim());
				return;
			} else{

				if(zoom <= 0){ // frequency filter 0Hz ~ 20kHz/(2^n)

					output->insert(output->end(), vHigh.Max());
					//output->insert(output->end(), vHigh.Min());
					output->insert(output->end(), vHigh.Norm(2)/vLow.Dim());
				}

				DWT(&vLow, --J, output, wavelet_type, --zoom);
			}
			
		}
	}

	return;

	/*
	for(int i = 0; i < signal->Dim(); i++){

		KALDI_LOG << "signal(" << i << ") = " << (*signal)(i);
	}
	
	for(int i = 0; i < length/2; i++){

		KALDI_LOG << "vLow(" << i << ") = " << vLow(i);
	}

	for(int i = 0; i < length/2; i++){

		KALDI_LOG << "vHigh(" << i << ") = " << vHigh(i);
	}
	*/
}

void WPT(VectorBase<BaseFloat> *signal, int J, std::vector<BaseFloat> *output, std::string wavelet_type, int zoom){

	/*
	if(signal->Dim() < 2 * pow(2, J - zoom)){

		KALDI_ERR << "444ghost.ERROR in feature-wavelet.cc: too many decomposition levels for current window size(sample size)";
		return;
	}
	*/
	Vector<BaseFloat> vSignal(*signal);
	int length = vSignal.Dim();
	Vector<BaseFloat> buffer(length);

	if(wavelet_type == "haar"){

		for(int n = 0; n < J; n++){ // the number of decomposition level J

			for(int m = 0; m < pow(2, n); m++){ // the number of subbands for each level m
				
				//KALDI_LOG << "m = " << m;
					
				Vector<BaseFloat> vLow(length/2), vHigh(length/2);

				for(int o = 0; o < length/2; o++){ // average and difference for all subbands at current J
	
					vLow(o) = (vSignal((2*o) + ((length)*m)) + vSignal((2*o) + ((length)*m) + 1)) / 2;
					vHigh(o) = (vSignal((2*o) + ((length)*m)) - vSignal((2*o) + ((length)*m) + 1)) / 2;
					buffer(o + ((length)*m) ) = vLow(o);
					buffer(o + ((length)*m) + (length/2)) = vHigh(o);
					//KALDI_LOG << "((length/2)*m) + o = " << ((length/2)*m) + o;
				}

				if(n >= zoom){ // frequency filter 0Hz ~ 20kHz/(2^n)

					if(zoom != 0){

						if(m < pow(2, n)/2){

							if((n == J - 1) && (m == 0)){ // last level first subband for average
							
								output->insert(output->end(), vLow.Max());
								//output->insert(output->end(), vLow.Min());
								output->insert(output->end(), vLow.Norm(2));
								//output->insert(output->end(), vHigh.Max());
								//output->insert(output->end(), vHigh.Min());
								//output->insert(output->end(), vHigh.Norm(2));
							} else{

								//output->insert(output->end(), vLow.Max());
								//output->insert(output->end(), vLow.Min());
								//output->insert(output->end(), vLow.Norm(2));
								output->insert(output->end(), vHigh.Max());
								//output->insert(output->end(), vHigh.Min());
								output->insert(output->end(), vHigh.Norm(2));
							}
						}
					} else{

						if((n == J - 1) && (m == 0)){ // last level first subband for average
							
							output->insert(output->end(), vLow.Max());
							//output->insert(output->end(), vLow.Min());
							output->insert(output->end(), vLow.Norm(2));
							//output->insert(output->end(), vHigh.Max());
							//output->insert(output->end(), vHigh.Min());
							//output->insert(output->end(), vHigh.Norm(2));
						} else{

							//output->insert(output->end(), vLow.Max());
							//output->insert(output->end(), vLow.Min());
							//output->insert(output->end(), vLow.Norm(2));
							output->insert(output->end(), vHigh.Max());
							//output->insert(output->end(), vHigh.Min());
							output->insert(output->end(), vHigh.Norm(2));
						}
					}
				}
				/*
				vLow.SetZero(); // re-assigning values in the for state above so may not be necessary
				vHigh.SetZero();
				
				for(int p = 0; p < buffer.Dim(); p++){

					KALDI_LOG << "buffer(" << p << ") = " << buffer(p);					
				}
				*/
			}
			
			vSignal.SetZero();
			vSignal.CopyFromVec(buffer);
			buffer.SetZero();
			length /= 2;
		}
	} else if(wavelet_type == "db4"){

		if(vSignal.Dim() > 3){

			float l0 = 0.4829629131445341, l1 = 0.8365163037378077, l2 = 0.2241438680420134, l3 = -0.1294095225512603;
			float h0 = l3, h1 = -l2, h2 = l1, h3 = -l0;

			for(int n = 0; n < J; n++){ // the number of decomposition level J

				for(int m = 0; m < pow(2, n); m++){ // the number of subbands for each level m
					
					Vector<BaseFloat> vLow(length/2), vHigh(length/2);

					for(int i = 0; i < length - 3; i = i + 2){ // instead of signal->Dim() - 2 for exactly 4 dimension long signal

						float low = 0, high = 0;

						low += vSignal(i + (length*m))*l0;
						low += vSignal((i+1) + (length*m))*l1;
						low += vSignal((i+2) + (length*m))*l2;
						low += vSignal((i+3) + (length*m))*l3;

						high += vSignal(i + (length*m))*h0;
						high += vSignal((i+1) + (length*m))*h1;
						high += vSignal((i+2) + (length*m))*h2;
						high += vSignal((i+3) + (length*m))*h3;
						
						vLow(i/2) = low;
						vHigh(i/2) = high;

						buffer((i/2) + (length*m)) = low;
						buffer((i/2) + (length*m) + (length/2)) = high;

						//KALDI_LOG << "buffer(" << (i/2) + (length*m) << ") = " << low;
						//KALDI_LOG << "buffer(" << (i/2) + (length*m) + (length/2) << ") = " << high;
					}

					float low = 0, high = 0;

					low += vSignal((length*(m+1)) - 2)*l0;
					low += vSignal((length*(m+1)) - 1)*l1;
					low += vSignal(length*m)*l2;
					low += vSignal((length*m) + 1)*l3;

					high += vSignal((length*(m+1)) - 2)*h0;
					high += vSignal((length*(m+1)) - 1)*h1;
					high += vSignal(length*m)*h2;
					high += vSignal((length*m) + 1)*h3;

					vLow((length/2) -1) = low;
					vHigh((length/2) -1) = high;

					buffer(((length/2)-1) + (length*m)) = low;
					buffer(((length/2)-1) + (length*m) + (length/2)) = high;

					/*
					KALDI_LOG << "buffer(" << ((length/2)-1) + (length*m) << ") = " << low;
					KALDI_LOG << "buffer(" << ((length/2)-1) + (length*m) + (length/2) << ") = " << high;
					KALDI_LOG << "-------------------";

					for(int i = 0; i < buffer.Dim(); i++){

						KALDI_LOG << "buffer(" << i << ") = " << buffer(i);
					}
					*/
					if(n >= zoom){ // frequency filter 0Hz ~ 20kHz/(2^n)

						if(zoom != 0){

							if(m < pow(2, n)/2){

								if((n == J - 1) && (m == 0)){ // last level first subband for average
								
									output->insert(output->end(), vLow.Max());
									//output->insert(output->end(), vLow.Min());
									output->insert(output->end(), vLow.Norm(2));
									//output->insert(output->end(), vHigh.Max());
									//output->insert(output->end(), vHigh.Min());
									//output->insert(output->end(), vHigh.Norm(2));
								} else{

									//output->insert(output->end(), vLow.Max());
									//output->insert(output->end(), vLow.Min());
									//output->insert(output->end(), vLow.Norm(2));
									output->insert(output->end(), vHigh.Max());
									//output->insert(output->end(), vHigh.Min());
									output->insert(output->end(), vHigh.Norm(2));
								}
							}
						} else{

							if((n == J - 1) && (m == 0)){ // last level first subband for average
								
								output->insert(output->end(), vLow.Max());
								//output->insert(output->end(), vLow.Min());
								output->insert(output->end(), vLow.Norm(2));
								//output->insert(output->end(), vHigh.Max());
								//output->insert(output->end(), vHigh.Min());
								//output->insert(output->end(), vHigh.Norm(2));
							} else{

								//output->insert(output->end(), vLow.Max());
								//output->insert(output->end(), vLow.Min());
								//output->insert(output->end(), vLow.Norm(2));
								output->insert(output->end(), vHigh.Max());
								//output->insert(output->end(), vHigh.Min());
								output->insert(output->end(), vHigh.Norm(2));
							}
						}
					}
					//KALDI_LOG << "output->size() = " << output->size();
				}

				vSignal.SetZero();
				vSignal.CopyFromVec(buffer);
				buffer.SetZero();
				length /= 2;
			}
		}
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

	int J = opts_.decomposition_level;
	std::vector<BaseFloat> output;
	Vector<BaseFloat> signal(*signal_frame);

	if(transform_type == "dwt"){

		DWT(&signal, J, &output, wavelet_type, opts_.dyadic_zoom);
		KALDI_LOG << "output.size() = " << output.size();
		//(*feature)(opts_.num_feats - 1) = signal.Norm(2);
	} else if(transform_type == "wpt"){

		/*
		Vector<BaseFloat> _10k(signal.Dim()/2);
		_10k.CopyFromVec(_10kFilter(&signal));
		*/
		WPT(&signal, J, &output, wavelet_type, opts_.dyadic_zoom);
		//(*feature)(opts_.num_feats - 1) = signal.Norm(2);
	}

	for(int i = 0; i < opts_.num_feats - 1; i++){

		(*feature)(i) = output.at(i);
		//KALDI_LOG << "444ghost.LOG in feature-wavelet.cc: (*feature)(" << i << ") = " << (*feature)(i);
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
	KALDI_LOG << "dyadic_zoom = " << opts.dyadic_zoom;
	KALDI_LOG << "<---------------444ghost.LOG in feature-wavelet.cc";
}

WaveletComputer::~WaveletComputer() {
 
}

}  // namespace kaldi
