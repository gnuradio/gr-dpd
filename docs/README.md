# gr-dpd: Digital Pre-Distortion
--------------------------------------

Digital Pre-Distortion is a popular technique used to compensate for distortion caused due to non-linear Power Amplifiers used in wireless communication today. This OOT(out-of-tree) module, namely *gr-dpd* is an implementation of some standard DPD Algorithms implemented in form of C++ blocks.

Some of the blocks implemented so far, in *gr-dpd*, are listed below (with description):

## MP model PA
   This block is basically an implementation of *Memory Polynomial* model based *Power Amplifier*. It produces an output based on the Memory Polynomial model formed with current block parameters for each input.
   
   Both Input and Ouput for this block is a complex data type in stream domain.
   
   Here is the function to describe relation between Input and Output of MP based model:
	
   ![MP_Model Relation](https://github.com/gnuradio/gr-dpd/blob/master/raw/MP_Model.jpg)<br>

### Parameters
(R): [Run-Time Adjustable](https://wiki.gnuradio.org/index.php/Guided_Tutorial_GRC#Variable_Controls)
    
   * **Order (K)**: *Maximum Power Order* or Nonlinearity Order of the terms in Memory Polynomial. 
   * **Memory Depth (M)**: Maximum number of terms of each power order in memory polynomial due to *Memory Depth*.
   * **Mode**: Mode of Operation, i.e., Odd Order Terms Only or Even Order Terms Only or Both Terms.
   * **Coefficient Vector (K x M)**: A shift-structured vector of complex data types of order (K x M) containing the coefficients for the MP Model.

   For test flowgraph, check [here](https://github.com/gnuradio/gr-dpd/tree/master/examples#mp_pa_test-flowgraph).

## GMP model PA
   This block is basically an implementation of *Generalised Memory Polynomial* model based *Power Amplifier*. It produces an output based on the Generalised Memory Polynomial model formed with current block parameters for each input.
   
   Both Input and Ouput for this block is a complex data type in stream domain.
   
   Here is the function to describe relation between Input and Output of GMP based model:

   <img src="https://github.com/gnuradio/gr-dpd/blob/master/raw/GMP_Model.jpg" height="50%" width="50%" /><br>

### Parameters
(R): [Run-Time Adjustable](https://wiki.gnuradio.org/index.php/Guided_Tutorial_GRC#Variable_Controls)

   * **K_a**: *Maximum Power Order* of terms in the *signal-and-aligned-envelope* component of the Generalised Memory Polynomial Model.
   * **L_a**: Maximum number of terms of each power order in the *signal-and-aligned-envelope* component of the Generalised Memory Polynomial due to *Memory Depth*.
   * **K_b**: *Maximum Power Order* of terms in the *signal-and-lagging-envelope component* of the Generalised Memory Polynomial Model.
   * **L_b**: Maximum number of terms of each power order in the *signal-and-lagging-envelope* component of the Generalised Memory Polynomial due to *Memory-Depth*.
   * **M_b**: *Lagging Cross-Terms Index* or Maximum number of terms due to *Lagging Cross Terms* for each memory depth in the *signal-and-lagging-envelope* component of the GMP Model.
   * **Mode**: Mode of Operation, i.e., Odd Order Terms Only or Even Order Terms Only or Both Terms.
   * **Coefficient Vector I (K_a x L_a)**: A shift-structured vector of complex data types of order (K_a x L_a) containing the coefficients for *signal-and-aligned-envelope* component of the GMP model.
   * **Coefficient Vector II (K_b x L_b x M_b)**: A shift-structured vector of complex data types of order (K_b x L_b x M_b) containing the coefficients for *signal-and-lagging-envelope* component of the GMP model.

   For test flowgraph, check [here](https://github.com/gnuradio/gr-dpd/tree/master/examples#gmp_pa_test-flowgraph).

## Predistorter Training
   This block performs *predistortion* of the input stream to be fed to the Power Amplifier (PA). It has to be necessarily supplied with the parameters (as complex vector) of the behavioral model (GMP) used for estimation of the PA model coefficients or predistortion coefficients (inverse of PA model estimated). It can operate in both Static and Training modes of predistortion.

* In static mode, predistortion coefficients are given by user as *complex_vector* parameter.
* In training mode, it is necessary to connect *'taps'* input message-port to a *postdistorter* block passing message *'taps'* after adaptive-estimation.

It multiplies GMP row vector for each input complex value with predistorter coefficients column vector (of same order) to give a predistorted input to PA. Mathematically, Output[m] = Input[m] * taps (where Input[m] is input GMP vector(row) and 'taps' is weight vector(col)).

### Parameters
(R): [Run-Time Adjustable](https://wiki.gnuradio.org/index.php/Guided_Tutorial_GRC#Variable_Controls)

   * **DPD Parameters**: The *(K_a, L_a, K_b, L_b, M_b)* int_vector denoting the GMP model parameters used to determine the order of GMP vector generated for each input for its predistortion.
   * **Mode**: Modes of Operation, i.e., *Training* or *Static*.
   * **Predistorter taps**: Predistortion coefficients or taps as complex_vector (Optional ,i.e., Parameter only in static mode).

   For test flowgraph, check [here](https://github.com/gnuradio/gr-dpd/tree/master/examples#static_predistorter_test_run).

## Gain Phase Calibrate
   This block is useful for removing the *gain* from the Power Amplifier Output to calibrate it for error calculation with the predistorter output (input to PA). 
It has three input ports, of which, first is the PA_DPD to be calibrated and the other two inputs, i.e., Input Sample and PA_Output are used to calculate the inverse gain multiplier. Both Input and Output Ports are only for Complex Data Type.

### Parameters
(R): [Run-Time Adjustable](https://wiki.gnuradio.org/index.php/Guided_Tutorial_GRC#Variable_Controls)

   Currently, there are no parameters in this block.

   For test flowgraph, check [here](https://github.com/gnuradio/gr-dpd/tree/master/examples#gain_phase_test).

## RLS Postdistorter
   RLS based Algorithm implemented to estimate the *coefficients* of the behaviorial model (GMP) of the Power Amplifier based on PA_output and PA_input in this block. It uses the fast RLS (Recursive Least Squares) Algorithm based on Hyperbolics and Givens Rotations.

It has two input ports, one for the PA_output (gain phase calibrated) and other for the PA_input (or predistorter output).
The coefficients estimated are passed as messages through message port *'taps'*. Both Input ports are only for Complex Data Type.

### Parameters
(R): [Run-Time Adjustable](https://wiki.gnuradio.org/index.php/Guided_Tutorial_GRC#Variable_Controls)

   * **DPD Parameters**: The *(K_a, L_a, K_b, L_b, M_b)* int_vector denoting the GMP model parameters used for predistorter coefficients estimations.
			 Total No. of coefficients = ((K_a * L_a) + (K_b * M_b * L_b))
   * **Iteration Limit**: Max. number of iterations of training to be performed for predistorter DPD coefficients estimation.

   For test flowgraph, check [here](https://github.com/gnuradio/gr-dpd/tree/master/examples#rls_test_run_two_tone_setup).

## LMS Postdistorter
   LMS based Algorithm implemented to estimate the *coefficients* of the behaviorial model (GMP) of the Power Amplifier based on PA_output and PA_input in this block. It uses the LMS (Least Mean Squares) Algorithm with two methods options, namely, *Newton based* and *EMA based* methods.

It has two input ports, one for the PA_output (gain phase calibrated) and other for the PA_input (or predistorter output).
The coefficients estimated are passed as messages through message port *'taps'*. Both Input ports are only for Complex Data Type.

### Parameters
(R): [Run-Time Adjustable](https://wiki.gnuradio.org/index.php/Guided_Tutorial_GRC#Variable_Controls)

   * **DPD Parameters**: The *(K_a, L_a, K_b, L_b, M_b)* int_vector denoting the GMP model parameters used for predistorter coefficients estimations.
			 Total No. of coefficients = ((K_a * L_a) + (K_b * M_b * L_b))
   * **Iteration Limit**: Max. number of iterations of training to be performed for predistorter DPD coefficients estimation.
   * **Method**: Method of LMS algorithm used for coefficients estimation, i.e., *Newton* or *EMA* based method.

   For test flowgraph, check [here](https://github.com/gnuradio/gr-dpd/tree/master/examples#lms_test_run_two_tone_setup).

## Stream_to_gmp_vector
   Converts a stream of single complex data items into corresponding GMP vectors based on GMP model parameters using input items history.
   Both Input and Output items are complex. 
	
   Input is a single item but output is of order M (length of GMP vector based on parameters).

### Parameters
(R): [Run-Time Adjustable](https://wiki.gnuradio.org/index.php/Guided_Tutorial_GRC#Variable_Controls)

   * **DPD Parameters**: The *(K_a, L_a, K_b, L_b, M_b)* int_vector denoting the GMP model parameters used to determine the order and generate the GMP vector corresponding to each input data item.

## Stream_to_message
   Converts a stream of single complex data item into messages (PMT's) which are published by output message port *'message'*.

### Parameters
(R): [Run-Time Adjustable](https://wiki.gnuradio.org/index.php/Guided_Tutorial_GRC#Variable_Controls)

   Currently, there are no parameters in this block.

