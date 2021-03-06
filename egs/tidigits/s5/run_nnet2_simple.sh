#!/bin/bash

# Joshua Meyer 2017
# This script is based off the run_nnet2_baseline.sh script from the wsj eg
# This is very much a toy example, intended to be for learning the ropes of 
# nnet2 training and testing in Kaldi. You will not get state-of-the-art
# results.
# The default parameters here are in general low, to make training and 
# testing faster on a CPU.

stage=1
experiment_dir=exp/nnet2/nnet2_simple
num_threads=4
minibatch_size=128
unknown_phone=SPOKEN_NOISE # having these explicit is just something I did when
silence_phone=SIL          # I was debugging, they are now required by decode.sh

. ./path.sh
. ./utils/parse_options.sh


if [ $stage -le 1 ]; then

    echo ""
    echo "######################"
    echo "### BEGIN TRAINING ###"
    echo "######################"

    mkdir -p $experiment_dir

    # --splice-width: subdivided frames from 4 to 2
    # --num-hidden-layers from 3 to 2
    # --initial-learning-rate from 0.02 to 0.04
    # --num-epochs from 10 to 20
# 444ghost ->
#SoftmaxComponent dimensions are larger than the configured initially then grouped up to config value.
#The number of dimensions correspond to the context-dependent triphone labels.
#./tools/nnet-am-info exp/nnet2/nnet2_simple/final.mdl
# 444ghost <-
    steps/nnet2/train_simple.sh \
        --stage -10 \
        --num-threads "$num_threads" \
        --feat-type raw \
        --splice-width 4 \
	--lda_dim 65 \
        --num-hidden-layers 2 \
        --hidden-layer-dim 250 \
        --add-layers-period 5 \
        --num-epochs 20 \
        --iters-per-epoch 1 \
        --initial-learning-rate 0.001 \
        --final-learning-rate 0.004 \
        --minibatch-size "$minibatch_size" \
        data/train \
        data/lang \
        exp/tri1 \
        $experiment_dir \
        || exit 1;

    echo ""
    echo "####################"
    echo "### END TRAINING ###"
    echo "####################"

fi


if [ $stage -le 2 ]; then

    echo ""
    echo "#####################"
    echo "### BEGIN TESTING ###"
    echo "#####################"

    steps/nnet2/decode_simple.sh \
        --num-threads "$num_threads" \
        --beam 8 \
        --max-active 500 \
        --lattice-beam 3 \
        exp/tri1/graph \
        data/test \
        $experiment_dir/final.mdl \
        $unknown_phone \
        $silence_phone \
        $experiment_dir/decode \
        || exit 1;

    for x in ${experiment_dir}/decode*; do
        [ -d $x ] && grep WER $x/wer_* | \
            utils/best_wer_nnet2.sh;
    done

    echo ""
    echo "###################"
    echo "### END TESTING ###"
    echo "###################"

fi
