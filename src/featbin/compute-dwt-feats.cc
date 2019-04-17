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
	ParseOptions po("444ghost.ERROR in compute-dwt-feats.cc: Invalid options for initializing ParseOptions ");
	WaveletOptions wavelet_opts;

	wavelet_opts.Register(&po);
	po.Read(argc, argv);

	KALDI_LOG << "444ghost.LOG in compute-dwt-feats.cc: num_feats = " << wavelet_opts.num_feats;
	KALDI_LOG << "444ghost.LOG in compute-dwt-feats.cc: wavelet_type = " << wavelet_opts.wavelet_type;
	KALDI_LOG << "444ghost.LOG in compute-dwt-feats.cc: decomposition_level = " << wavelet_opts.decomposition_level;

	// Define defaults for gobal options
	std::string output_format = "kaldi";
	int32 channel = -1; // -1 mono, 0 left, 1 right
	BaseFloat min_duration = 0.0;
	std::string wav_rspecifier = po.GetArg(1);
	std::string output_wspecifier = po.GetArg(2);

	Wavelet wavelet(wavelet_opts);

	SequentialTableReader<WaveHolder> reader(wav_rspecifier);
    	BaseFloatMatrixWriter kaldi_writer;

	if (output_format == "kaldi") {
		if (!kaldi_writer.Open(output_wspecifier))
			KALDI_ERR << "Could not initialize output with wspecifier " << output_wspecifier;
	} else{

		KALDI_ERR << "444ghost.ERROR in compute-dwt-feats.cc: output_format is not \"kaldi\"";
	}
	
	int32 num_utts = 0, num_success = 0;

	for (; !reader.Done(); reader.Next()) {
		num_utts++;
		std::string utt = reader.Key();
		const WaveData &wave_data = reader.Value();
		if (wave_data.Duration() < min_duration) {
			KALDI_WARN << "File: " << utt << " is too short ("
				   << wave_data.Duration() 
				   << " sec): producing no output.";
			continue;
		}

		int32 num_chan = wave_data.Data().NumRows(), this_chan = channel;
		{  	// This block works out the channel (0=left, 1=right...)
			KALDI_ASSERT(num_chan > 0);  // should have been caught in
			// reading code if no channels.
			if (channel == -1) {
				this_chan = 0;
				if (num_chan != 1)
					KALDI_WARN << "Channel not specified but you have data with "
				        	   << num_chan  
						   << " channels; defaulting to zero";
			} else {
				if (this_chan >= num_chan) {
					KALDI_WARN << "File with id " << utt << " has "
				                   << num_chan 
						   << " channels but you specified channel "
				                   << channel 
						   << ", producing no output.";
		    			continue;
				}
			}
		}

		SubVector<BaseFloat> waveform(wave_data.Data(), this_chan);
		Matrix<BaseFloat> features;
		try {
/********************************************************************************/
			//mfcc.ComputeFeatures(waveform, wave_data.SampFreq(), vtln_warp_local, &features);
/********************************************************************************/
		} catch (...) {
			KALDI_WARN << "Failed to compute features for utterance "
			           << utt;
			continue;
		}

		if (num_utts % 10 == 0)
			KALDI_LOG << "Processed " << num_utts << " utterances";
		KALDI_VLOG(2) << "Processed features for key " << utt;
		num_success++;
	}
	KALDI_LOG << " Done " << num_success << " out of " << num_utts
                  << " utterances.";
	return (num_success != 0 ? 0 : 1);
}

