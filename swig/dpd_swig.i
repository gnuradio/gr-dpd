/* -*- c++ -*- */

#define DPD_API

%include "gnuradio.i"           // the common stuff

//load generated python docstrings
%include "dpd_swig_doc.i"

%{
#include "dpd/MP_model_PA.h"
%}

%include "dpd/MP_model_PA.h"
GR_SWIG_BLOCK_MAGIC2(dpd, MP_model_PA);
