#!/bin/bash

# Copyright 2012-2016  Johns Hopkins University (Author: Daniel Povey)
# Apache 2.0
# To be run from .. (one directory up from here)
# see ../run.sh for example

# Begin configuration section.
nj=4
cmd=run.pl
wavelet_config=conf/wavelet.conf # 444ghost
compress=true
write_utt2num_frames=false  # if true writes utt2num_frames
# End configuration section.

echo "$0 $@"  # Print the command line for logging

if [ -f path.sh ]; then . ./path.sh; fi
. parse_options.sh || exit 1;

# 444ghost ->
data=$1
if [ $# -ge 2 ]; then
  logdir=$2
else
  logdir=$data/log
fi
if [ $# -ge 3 ]; then
  waveletdir=$3 
else
  waveletdir=$data/data
fi
trasnform_type=$waveletdir
# 444ghost <-

if [ $# -lt 1 ] || [ $# -gt 3 ]; then
   echo "Usage: $0 [options] <data-dir> [<log-dir> [<wavelet-dir>] ]";
   echo "e.g.: $0 data/train exp/make_$trasnform_type/train $trasnform_type" # 444ghost
   echo "Note: <log-dir> defaults to <data-dir>/log, and <waveletdir> defaults to <data-dir>/data"
   echo "Options: "
   echo "  --wavelet-config <config-file>                   # config passed to compute-wavelet-feats " # 444ghost
   echo "  --nj <nj>                                        # number of parallel jobs"
   echo "  --cmd (utils/run.pl|utils/queue.pl <queue opts>) # how to run jobs."
   echo "  --write-utt2num-frames <true|false>              # If true, write utt2num_frames file."
   exit 1;
fi

# make $waveletdir an absolute pathname.
waveletdir=`perl -e '($dir,$pwd)= @ARGV; if($dir!~m:^/:) { $dir = "$pwd/$dir"; } print $dir; ' $waveletdir ${PWD}` # 444ghost

# use "name" as part of name of the archive.
name=`basename $data`

mkdir -p $waveletdir || exit 1; # 444ghost
mkdir -p $logdir || exit 1;

if [ -f $data/feats.scp ]; then
  mkdir -p $data/.backup
  echo "$0: moving $data/feats.scp to $data/.backup"
  mv $data/feats.scp $data/.backup
fi

scp=$data/wav.scp

required="$scp $wavelet_config" # 444ghost

for f in $required; do
  if [ ! -f $f ]; then
    echo "make_wavelet.sh: no such file $f"
    exit 1;
  fi
done
utils/validate_data_dir.sh --no-text --no-feats $data || exit 1;

if [ -f $data/spk2warp ]; then
  echo "$0 [info]: using VTLN warp factors from $data/spk2warp"
  vtln_opts="--vtln-map=ark:$data/spk2warp --utt2spk=ark:$data/utt2spk"
elif [ -f $data/utt2warp ]; then
  echo "$0 [info]: using VTLN warp factors from $data/utt2warp"
  vtln_opts="--vtln-map=ark:$data/utt2warp"
else
  vtln_opts=""
fi

for n in $(seq $nj); do
  # the next command does nothing unless $waveletdir/storage/ exists, see
  # utils/create_data_link.pl for more info.
  utils/create_data_link.pl $waveletdir/raw_${trasnform_type}_$name.$n.ark # 444ghost
done


if $write_utt2num_frames; then
  write_num_frames_opt="--write-num-frames=ark,t:$logdir/utt2num_frames.JOB"
else
  write_num_frames_opt=
fi


if [ -f $data/segments ]; then
  # 444ghost ->
  echo "444ghost.ERROR in make_wavelet.sh: Segment file exists, what?..."
  exit 1
  # 444ghost <-
else
  echo "$0: [info]: no segments file exists: assuming wav.scp indexed by utterance."
  split_scps=""
  for n in $(seq $nj); do
    split_scps="$split_scps $logdir/wav_${name}.$n.scp"
  done

  utils/split_scp.pl $scp $split_scps || exit 1;


  # add ,p to the input rspecifier so that we can just skip over
  # utterances that have bad wave data.

  # 444ghost ->
  $cmd JOB=1:$nj $logdir/make_${trasnform_type}_${name}.JOB.log \
    compute-${trasnform_type}-feats  $vtln_opts --verbose=2 --config=$wavelet_config \
     scp,p:$logdir/wav_${name}.JOB.scp ark:- \| \
      copy-feats $write_num_frames_opt --compress=$compress ark:- \
      ark,scp:$waveletdir/raw_${trasnform_type}_$name.JOB.ark,$waveletdir/raw_${trasnform_type}_$name.JOB.scp \
      || exit 1;
  # 444ghost <-
fi


if [ -f $logdir/.error.$name ]; then
  echo "Error producing ${trasnform_type} features for $name:"
  tail $logdir/make_${trasnform_type}_${name}.1.log
  exit 1;
fi

# concatenate the .scp files together.
for n in $(seq $nj); do
  cat $waveletdir/raw_${trasnform_type}_$name.$n.scp || exit 1; # 444ghost
done > $data/feats.scp || exit 1

if $write_utt2num_frames; then
  for n in $(seq $nj); do
    cat $logdir/utt2num_frames.$n || exit 1;
  done > $data/utt2num_frames || exit 1
  rm $logdir/utt2num_frames.*
fi

rm $logdir/wav_${name}.*.scp  $logdir/segments.* 2>/dev/null

nf=`cat $data/feats.scp | wc -l`
nu=`cat $data/utt2spk | wc -l`
if [ $nf -ne $nu ]; then
  echo "It seems not all of the feature files were successfully processed ($nf != $nu);"
  echo "consider using utils/fix_data_dir.sh $data"
fi

if [ $nf -lt $[$nu - ($nu/20)] ]; then
  echo "Less than 95% the features were successfully generated.  Probably a serious error."
  exit 1;
fi

echo "Succeeded creating ${trasnform_type} features for $name"
