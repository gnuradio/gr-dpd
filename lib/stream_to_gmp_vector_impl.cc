/* -*- c++ -*- */
/*
 * Copyright 2020 gr-dpd author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "stream_to_gmp_vector_impl.h"
#include <armadillo>
#include <algorithm>
#include <math.h>
#include <dpd/dpd_utils.h>
#define COPY_MEM false  // Do not copy vectors into separate memory
#define FIX_SIZE true // Keep dimensions of vectors constant

using std::vector;
using namespace arma;

namespace gr {
  namespace dpd {

    stream_to_gmp_vector::sptr
    stream_to_gmp_vector::make(const std::vector<int> &dpd_params)
    {
      return gnuradio::get_initial_sptr
        (new stream_to_gmp_vector_impl(dpd_params));
    }


    /*
     * The private constructor
     */
    stream_to_gmp_vector_impl::stream_to_gmp_vector_impl(const std::vector<int> &dpd_params)
      : gr::sync_block("stream_to_gmp_vector",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, (dpd_params[0]*dpd_params[1] + dpd_params[2]*dpd_params[3]*dpd_params[4])*sizeof(gr_complex))),
              d_dpd_params(dpd_params),
              K_a(d_dpd_params[0]),
              L_a(d_dpd_params[1]),
              K_b(d_dpd_params[2]),
              M_b(d_dpd_params[3]),
              L_b(d_dpd_params[4]),
              M(dpd_params[0]*dpd_params[1] + dpd_params[2]*dpd_params[3]*dpd_params[4])     
    {
      set_history( std::max(L_a, M_b+L_b) );  
    }

    /*
     * Our virtual destructor.
     */
    stream_to_gmp_vector_impl::~stream_to_gmp_vector_impl()
    {
    }

    int
    stream_to_gmp_vector_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      // const <+ITYPE+> *in = (const <+ITYPE+> *) input_items[0];
      // <+OTYPE+> *out = (<+OTYPE+> *) output_items[0];

      // Do <+signal processing+>
      for (int item = history()-1; item < noutput_items+history()-1; item++) 
      {         
        cx_fcolvec GMP_vector( ((gr_complex *) output_items[0])+(item-history()+1)*(M), M, COPY_MEM, FIX_SIZE );
        gen_GMPvector((const gr_complex *) input_items[0], item, K_a, L_a, K_b, M_b, L_b, GMP_vector);
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace dpd */
} /* namespace gr */

