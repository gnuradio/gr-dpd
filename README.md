# gr-dpd: Digital Pre-Distortion
--------------------------------------

## Overview:
<img align="right" src="https://wiki.gnuradio.org/images/2/2f/Gnuradio_logo_icon.png?054f8" width="12%" />

The gr-dpd module is a GNU Radio implementation of Digital Pre-Distortion, a popular technique used to compensate for 
distortion caused due to non-linear Power Amplifiers used in wireless communication today.

This has been developed as a [GSoC 2020 project](https://summerofcode.withgoogle.com/projects/#6468946162614272)
under [GNU Radio](https://summerofcode.withgoogle.com/organizations/6453828414603264/).

Major DPD Algorithms considered for this module are:

* Fast RLS based Algorithm 
* LMS (Least Mean Squares) Algorithm
* Look-Up Table based Algorithm

So, far RLS and LMS based algorithm has been implemented. 

The blocks added to the module, so far, are:

* [GMP_model_PA](https://github.com/gnuradio/gr-dpd/tree/master/docs#gmp-model-pa)
* [MP_model_PA](https://github.com/gnuradio/gr-dpd/tree/master/docs#mp-model-pa)
* [predistorter_training](https://github.com/gnuradio/gr-dpd/tree/master/docs#predistorter-training)
* [RLS_postdistorter](https://github.com/gnuradio/gr-dpd/tree/master/docs#rls-postdistorter)
* [gain_phase_calibrate](https://github.com/gnuradio/gr-dpd/tree/master/docs#gain-phase-calibrate)
* [LMS_postdistorter](https://github.com/gnuradio/gr-dpd/tree/master/docs#lms-postdistorter)
* [stream_to_message](https://github.com/gnuradio/gr-dpd/tree/master/docs#stream_to_message)
* [stream_to_gmp_vector](https://github.com/gnuradio/gr-dpd/tree/master/docs#stream_to_gmp_vector)

## Dependencies:

gr-dpd requires:

`GNU Radio` version = 3.8  

`Armadillo` version >= 6.7

`CMake` version >= 3.8

`gcc` version >= 7.4.0

## Installation:

First, you need to install the dependencies (above).

Then, you need to download this repository
```bash
$ git clone https://github.com/gnuradio/gr-dpd.git
```

After this, gr-dpd should be installed as any other GNU Radio out-of-tree module.
```bash
$ cd gr-dpd
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
$ sudo ldconfig
```
If your GNU Radio is installed in other directory (rather than /usr/local), then replace the cmake line 
`cmake ..` above with: `cmake -DCMAKE_INSTALL_PREFIX=<directory of GNU Radio> ..`

## Working with the module:

All the progress related details of the development of this module and its proper record and resources related
to this module can be found [here](https://grdpd.wordpress.com/).

* Description of [Example Flowgraphs](https://github.com/gnuradio/gr-dpd/tree/master/examples) can be seen [here](https://github.com/gnuradio/gr-dpd/tree/master/examples#gr-dpd-example-flowgraphs).
* [Documentation](https://github.com/gnuradio/gr-dpd/tree/master/docs) of the gr-dpd, containing each block's description is given [here](https://github.com/gnuradio/gr-dpd/tree/master/docs#gr-dpd-digital-pre-distortion).

## Examples:

Various example flowgraphs have been added so far to demonstrate the functioning of gr-dpd blocks.


Below is the flowgraph demonstrating an RLS based pre-distortion run on a GMP model Power Amplifier:

![Test.grc - flowgraph](https://github.com/gnuradio/gr-dpd/blob/master/raw/RLS_test_run_two_tone_setup.jpg)<br>

## Bugs reporting:
Kindly report any bugs or issues [on Github](https://github.com/gnuradio/gr-dpd/issues/).

## Contributing to the project
If you want to contribute to the module, feel free to add the pull request.
Please read the contribution guidelines of GNU Radio [here](https://wiki.gnuradio.org/index.php/Development).

## Contact
For queries or feedback, drop a mail to [discuss-gnuradio](mailto:discuss-gnuradio@gnu.org).

## License
The project is licensed under GPLv3. See LICENSE for terms.


