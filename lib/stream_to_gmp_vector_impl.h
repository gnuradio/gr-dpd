/* -*- c++ -*- */
/*
 * Copyright 2020 gr-dpd author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DPD_STREAM_TO_GMP_VECTOR_IMPL_H
#define INCLUDED_DPD_STREAM_TO_GMP_VECTOR_IMPL_H

#include <dpd/stream_to_gmp_vector.h>

namespace gr {
  namespace dpd {

    class stream_to_gmp_vector_impl : public stream_to_gmp_vector
    {
     private:
      std::vector<int> d_dpd_params;
      //consts to be assigned values in the constructor initialization list
      const int K_a;
      const int L_a;
      const int K_b;
      const int M_b;
      const int L_b;
      const int M;      

     public:
      stream_to_gmp_vector_impl(const std::vector<int> &dpd_params);
      ~stream_to_gmp_vector_impl();

      // Where all the action really happens
      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );
    };

  } // namespace dpd
} // namespace gr

#endif /* INCLUDED_DPD_STREAM_TO_GMP_VECTOR_IMPL_H */

