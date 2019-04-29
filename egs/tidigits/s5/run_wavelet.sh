#!/bin/bash

# ./run_wavelet.sh -t=dwt|wpt
# ./run_wavelet.sh -transform=dwt|wpt

# Note: this TIDIGITS setup has not been tuned at all and has some obvious
# deficiencies; this has been created as a starting point for a tutorial.
# We're just using the "adults" data here, not the data from children.

rm -r dwt wpt data exp # 444ghost

. ./cmd.sh ## You'll want to change cmd.sh to something that will work on your system.
           ## This relates to the queue.

# This is a shell script, but it's recommended that you run the commands one by
# one by copying and pasting into the shell.

#tidigits=/export/corpora5/LDC/LDC93S10
#tidigits=/mnt/matylda2/data/TIDIGITS
tidigits=/home/sung/kaldi/kaldi/egs/tidigits/s5/tidigit_data/tidigits

# The following command prepares the data/{train,dev,test} directories.

local/tidigits_data_prep.sh $tidigits || exit 1;
local/tidigits_prepare_lang.sh  || exit 1;
utils/validate_lang.pl data/lang/ # Note; this actually does report errors,

# and exits with status 1, but we've checked them and seen that they
# don't matter (this setup doesn't have any disambiguation symbols,
# and the script doesn't like that).

# Now make wavelet features.
# waveletdir should be some place with a largish disk where you
# want to store wavelet features.

# 444ghost ->
numArg=$@
if [ -z "$numArg" ]; then
	echo "444ghost.ERROR in run_wavelet.sh: Specify transform type directory -t=dwt|wpt (-transform=dwt|wpt)"
	exit 1
fi

for i in "$@"; do
	case $i in
		-t=*|--transform=*)
		TRANSFORM="${i#*=}"

		;;
	esac
done
echo "444ghost.LOG in run_wavelet.sh: The transform type is ${TRANSFORM}"
waveletdir=${TRANSFORM}
for x in test train; do
	steps/make_wavelet.sh --cmd "$train_cmd" --nj 20 data/$x exp/make_$waveletdir/$x $waveletdir || exit 1; # 444ghost nj from 20 to 1
	utils/fix_data_dir.sh data/$x
	steps/compute_cmvn_stats.sh data/$x exp/make_$waveletdir/$x $waveletdir || exit 1;
done
# 444ghost <-

utils/subset_data_dir.sh data/train 1000 data/train_1k

# try --boost-silence 1.25 to some of the scripts below (also 1.5, if that helps...
# effect may not be clear till we test triphone system.  See
# wsj setup for examples (../../wsj/s5/run.sh)

steps/train_mono.sh --nj 4 --cmd "$train_cmd" data/train_1k data/lang exp/mono0a # 444ghost nj from 4 to 1
utils/mkgraph.sh data/lang exp/mono0a exp/mono0a/graph && steps/decode.sh --nj 10 --cmd "$decode_cmd" exp/mono0a/graph data/test exp/mono0a/decode # 444ghost nj from 10 to 1

steps/align_si.sh --nj 4 --cmd "$train_cmd" data/train data/lang exp/mono0a exp/mono0a_ali # 444ghost nj from 4 to 1
steps/train_deltas.sh --cmd "$train_cmd" 300 3000 data/train data/lang exp/mono0a_ali exp/tri1
utils/mkgraph.sh data/lang exp/tri1 exp/tri1/graph
steps/decode.sh --nj 10 --cmd "$decode_cmd" exp/tri1/graph data/test exp/tri1/decode # 444ghost nj from 10 to 1

# Example of looking at the output.
# utils/int2sym.pl -f 2- data/lang/words.txt  exp/tri1/decode/scoring/19.tra | sed "s/ $//" | sort | diff - data/test/text

# Getting results [see RESULTS file]
# for x in exp/*/decode*; do [ -d $x ] && grep SER $x/wer_* | utils/best_wer.sh; done

#exp/mono0a/decode/wer_17:%SER 3.67 [ 319 / 8700 ]
#exp/tri1/decode/wer_19:%SER 2.64 [ 230 / 8700 ]
