(검색용 텍스트: 칼디, 웨이블릿, 음성인식)

Wavelet Transform Feature Extraction Module for Kaldi
============
Kaldi[1] doesn't have any wavelet transform modules but files contained in this repository should help you extract acoustic features using wavelet transforms. This work is still in progress for better WERs.

The conf/wavelet.conf file allows you to change wavelet tranform options.
 - --wavelet-type // wavelet type currently supports haar and db4
 - --decomposition-level // decomposition level
 - --num-feats // number of features
 - --transform-type // discrete wavelet transform(dwt) or wavelet packet transform(wpt)
 - --dyadic-zoom // wpt's time-frequency resolution localization factor, can be 0 for no effect

--transform-type option should match -t option of the scripts below.

Kaldi version: 5.5.268 77ac79f70 

Data
----
TIdigits[2] data is used.

Scripts
-------
./run_wavelet.sh -t=dwt

./run_wavelet.sh -t=wpt

-t options should match transform-type in conf/wavelet.conf

Papers and tutorials
--------------------
Cubic-root log in PLP for intensity vs perceived loudness [3].

Wavelet features [4].

Kaldi tutorials [5], [6].

VM symbolic link on Windows [7].

Neural network tutorials for Kaldi [8].

Daubechies family of wavelets [9], [10].
 
Reference
---------
[1] https://github.com/kaldi-asr/kaldi

[2] https://catalog.ldc.upenn.edu/LDC93S10

[3] https://pdfs.semanticscholar.org/b578/f4faeb00b808e8786d897447f2493b12b4e9.pdf

[4] http://ecsjournal.org/Archive/Volume37/Issue3/7.pdf

[5] https://www.eleanorchodroff.com/

[6] http://www.inf.ed.ac.uk/teaching/courses/asr/2018-19/lab1.pdf
   
[7] https://www.nextofwindows.com/virtualbox-unable-to-merge-not-enough-free-storage-space

[8] http://jrmeyer.github.io/asr/2016/12/15/DNN-AM-Kaldi.html

[9] http://bearcave.com/misl/misl_tech/wavelets/matrix/index.html

[10] http://bearcave.com/misl/misl_tech/wavelets/daubechies/index.html

