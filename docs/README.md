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
   * **Mode**: Mode of Operation, i.e., Odd Order Terms or Even Order Terms Only or Both Terms.
   * **Coefficient Vector (K x M)**: A shift-structured vector of complex data types of order (K x M) containing the coefficients for the Memory Polynomial Model. 

   For test flowgraph, check [here](https://github.com/gnuradio/gr-dpd/tree/master/examples#mp_pa_test-flowgraph)

## GMP model PA:
   This block is basically an implementation of *Generalised Memory Polynomial* model based *Power Amplifier*. It produces an output based on the Generalised Memory Polynomial model formed with current block parameters for each input.
   
   Both Input and Ouput for this block is a complex data type in stream domain.
   
   Here is the function to describe relation between Input and Output of GMP based model:

   <img src="https://github.com/gnuradio/gr-dpd/blob/master/raw/GMP_Model.jpg" height="55%" width="55%" /><br>

### Parameters
(R): [Run-Time Adjustable](https://wiki.gnuradio.org/index.php/Guided_Tutorial_GRC#Variable_Controls)

   * **K_a**: *Maximum Power Order* of terms in the *signal-and-aligned-envelope* component of the Generalised Memory Polynomial Model.
   * **L_a**: Maximum number of terms of each power order in the *signal-and-aligned-envelope* component of the Generalised Memory Polynomial Model due to *Memory Depth*.
   * **K_b**: *Maximum Power Order* of terms in the *signal-and-lagging-envelope component* of the Generalised Memory Polynomial Model.
   * **L_b**: Maximum number of terms of each power order in the *signal-and-lagging-envelope* component of the Generalised Memory Polynomial Model due to *Memory-Depth*.
   * **M_b**: *Lagging Cross-Terms Index* or Maximum number of terms due to *Lagging Cross Terms* for each memory depth in the *signal-and-lagging-envelope* component of the Generalised Memory Polynomial Model.

   For test flowgraph, check [here](https://github.com/gnuradio/gr-dpd/tree/master/examples#gmp_pa_test-flowgraph).
