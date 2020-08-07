# gr-dpd Example flowgraphs

------------------------------

## GMP_PA_Test Flowgraph: 

------------------------------

This flowgraph uses a simple *two tone* test set up consisting of two *signal sources* of opposite frequencies and one *gaussian noise* source added up to test the working of GMP based Power Amplifier block for various values of the parameters: *K_a, L_a, K_b, M_B, L_b* of GMP model and various Modes of Operations.

Below is the flowgraph screenshot:

![GMP_PA_Test](https://github.com/gnuradio/gr-dpd/blob/master/raw/GMP_PA_Test.jpg)<br>

## MP_PA_Test Flowgraph:

------------------------------

This flowgraph uses a simple *two tone* setup similar above to test the working of MP based Power Amplifier block for various values of parameters:
*Order & Memory Length* and various Modes of Operations.

Below is the flowgraph screenshot:

![MP_PA_Test](https://github.com/gnuradio/gr-dpd/blob/master/raw/MP_PA_Test.jpg)<br>

## RLS_test_run_single_tone:

-------------------------------

This flowgraph is useful to observe the effect of the run of RLS algorithm on a simple *signal source* implemented using the blocks: 

* *stream_to_gmp_vector*
* *predistorter_training*
* *RLS_postdistorter*
* *gain_phase_calibrate*

It helps to observe the RLS training algorithm effects on a single tone.

Below is the flowgraph screenshot:

![RLS_test_run_single_tone](https://github.com/gnuradio/gr-dpd/blob/master/raw/RLS_test_run_single_tone.jpg)<br>


## RLS_test_run_two_tone_setup:

--------------------------------

This flowgraph is useful to observe some clear effects of Pre-distortion on a *two tone* signal setup consisting of two *signal sources* of opposite frequencies and one *gaussian noise* source, achieved using the RLS training algorithm implemented using the blocks:

* *stream_to_gmp_vector*
* *predistorter_training*
* *RLS_postdistorter*
* *gain_phase_calibrate*

Below is the flowgraph screenshot:

![RLS_test_run_two_tone_setup](https://github.com/gnuradio/gr-dpd/blob/master/raw/RLS_test_run_two_tone_setup.jpg)<br>

Here are some output screenshots on a *Frequency Sink* depicting the DPD effects on running the RLS Algorithm:

![RLS_DPD_output-1.jpg](https://github.com/gnuradio/gr-dpd/blob/master/raw/RLS_DPD_output-1.png)<br>

![RLS_DPD_output-2.jpg](https://github.com/gnuradio/gr-dpd/blob/master/raw/RLS_DPD_output-2.png)<br>




