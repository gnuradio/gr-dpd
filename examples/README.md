# gr-dpd Example flowgraphs

------------------------------

## MP_PA_Test Flowgraph:

------------------------------

This flowgraph uses a simple *two tone* setup similar above to test the working of MP based Power Amplifier block for various values of parameters:
*Order (K) & Memory Length (L)* and various Modes of Operations. Also to test the effects of different values of Coefficient Vector.

Below is the flowgraph screenshot:

![MP_PA_Test](https://github.com/gnuradio/gr-dpd/blob/master/raw/MP_PA_Test.jpg)<br>

## GMP_PA_Test Flowgraph: 

------------------------------

This flowgraph uses a simple *two tone* test set up consisting of two *signal sources* of opposite frequencies and one *gaussian noise* source added up to test the working of GMP based Power Amplifier block for various values of the parameters: *K_a, L_a, K_b, M_B, L_b* of GMP model and various Modes of Operations. Also to test the effects of different values of Coefficient Vectors I & II.

Below is the flowgraph screenshot:

![GMP_PA_Test](https://github.com/gnuradio/gr-dpd/blob/master/raw/GMP_PA_Test.jpg)<br>

## RLS_test_run_single_tone:

-------------------------------

This flowgraph is useful to understand the orientation and connections of the blocks used for DPD implementation in GRC & observing the effect of the run of RLS algorithm on a simple *signal source* implemented using the blocks: 

* *predistorter_training*
* *RLS_postdistorter*
* *gain_phase_calibrate*

Same flowgraph workflow will work fine for LMS_postdistorter on replacing the RLS_postdistorter.

Below is the flowgraph screenshot:

![RLS_test_run_single_tone](https://github.com/gnuradio/gr-dpd/blob/master/raw/RLS_test_run_single_tone.jpg)<br>


## RLS_test_run_two_tone_setup:

--------------------------------

This flowgraph is useful to observe some clear effects of Pre-distortion on a *two tone* signal setup consisting of two *signal sources* of opposite frequencies and one *gaussian noise* source, achieved using the RLS training algorithm implemented using the blocks:

* *predistorter_training*
* *RLS_postdistorter*
* *gain_phase_calibrate*

Below is the flowgraph screenshot:

![RLS_test_run_two_tone_setup](https://github.com/gnuradio/gr-dpd/blob/master/raw/RLS_test_run_two_tone_setup.jpg)<br>

Here are some output screenshots on a *Frequency Sink* depicting the DPD effects on running the RLS Algorithm:

![RLS_DPD_output-1](https://github.com/gnuradio/gr-dpd/blob/master/raw/RLS_DPD_output-1.png)<br>

![RLS_DPD_output-2](https://github.com/gnuradio/gr-dpd/blob/master/raw/RLS_DPD_output-2.png)<br>

## LMS_test_run_two_tone_setup:

--------------------------------

This flowgraph is useful to observe some clear effects of Pre-distortion on a *two tone* signal setup consisting of two *signal sources* of opposite frequencies and one *gaussian noise* source, achieved using the LMS training algorithm implemented using the blocks:

* *predistorter_training*
* *LMS_postdistorter*
* *gain_phase_calibrate*

Below is the flowgraph screenshot:

![LMS_test_run_two_tone_setup](https://github.com/gnuradio/gr-dpd/blob/master/raw/LMS_test_run_two_tone_setup.jpg)<br>

## gain_phase_test:

--------------------------------

This flowgraph uses a simple *signal source* and *multiply by constant* block to test the proper working of *gain_phase_calibrate* block.

Below is the flowgraph screenshot:

![gain_phase_test](https://github.com/gnuradio/gr-dpd/blob/master/raw/gain_phase_test.jpg)<br>

## gain_phase_GMP_test:

--------------------------------

This flowgraph uses a simple *signal source* and *GMP_model_PA* block's output to test the proper working of *gain_phase_calibrate* block.

Below is the flowgraph screenshot:

![gain_phase_test](https://github.com/gnuradio/gr-dpd/blob/master/raw/gain_phase_GMP_test.jpg)<br>

## Static_Predistorter_test_run:

--------------------------------

This flowgraph is useful to test the proper function of *predistorter* block in *static* mode of operation for different values of Predistorter *'taps'*.

Below is the flowgraph screenshot:

![Static_Predistorter_test_run](https://github.com/gnuradio/gr-dpd/blob/master/raw/Static_Predistorter_test_run.jpg)<br>
