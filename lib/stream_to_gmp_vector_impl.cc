/* -*- c++ -*- */
/*
 * Copyright 2020 gr-dpd author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "stream_to_gmp_vector_impl.h"
#include <gnuradio/io_signature.h>
#include <math.h>
#include <algorithm>
#include <armadillo>

#define COPY_MEM false // Do not copy vectors into separate memory
#define FIX_SIZE true  // Keep dimensions of vectors constant

using std::vector;
using namespace arma;

namespace gr {
namespace dpd {

stream_to_gmp_vector::sptr stream_to_gmp_vector::make(const std::vector<int>& dpd_params)
{
    return gnuradio::get_initial_sptr(new stream_to_gmp_vector_impl(dpd_params));
}


/*
 * The private constructor
 */
stream_to_gmp_vector_impl::stream_to_gmp_vector_impl(const std::vector<int>& dpd_params)
    : gr::sync_block(
          "stream_to_gmp_vector",
          gr::io_signature::make(1, 1, sizeof(gr_complex)),
          gr::io_signature::make(1,
                                 1,
                                 (dpd_params[0] * dpd_params[1] +
                                  dpd_params[2] * dpd_params[3] * dpd_params[4]) *
                                     sizeof(gr_complex))),
      d_dpd_params(dpd_params),
      K_a(d_dpd_params[0]),
      L_a(d_dpd_params[1]),
      K_b(d_dpd_params[2]),
      M_b(d_dpd_params[3]),
      L_b(d_dpd_params[4]),
      M(dpd_params[0] * dpd_params[1] + dpd_params[2] * dpd_params[3] * dpd_params[4])
{
    set_history(std::max(L_a, M_b + L_b));
}

/*
 * Our virtual destructor.
 */
stream_to_gmp_vector_impl::~stream_to_gmp_vector_impl() {}
void stream_to_gmp_vector_impl::gen_GMPvector(const gr_complex* const in,
                                       int item,
                                       int K_a,
                                       int L_a,
                                       int K_b,
                                       int M_b,
                                       int L_b,
                                       cx_fcolvec& GMP_vector)
{
    /* Signal-and-Aligned Envelope */
    // stacking L_a elements in reverse order
    cx_fcolvec y_vec_arma1(L_a, fill::zeros);
    for (int ii = L_a - 1; ii >= 0; ii--) {
        y_vec_arma1(ii) = in[item - ii];
    }
    GMP_vector.rows(0, L_a - 1) = y_vec_arma1;

    // store abs() of y_vec_arma1
    cx_fcolvec abs_y_vec_arma1(size(y_vec_arma1), fill::zeros);
    abs_y_vec_arma1.set_real(abs(y_vec_arma1));

    cx_fcolvec yy_temp;
    yy_temp = y_vec_arma1 % abs_y_vec_arma1;
    if (K_a > 1)
        GMP_vector.rows(L_a, 2 * L_a - 1) = yy_temp;
    for (int kk = 2; kk < K_a; kk++) {
        // perform element-wise product using the overloaded % operator
        yy_temp = yy_temp % abs_y_vec_arma1;

        GMP_vector.rows(kk * L_a, (kk + 1) * L_a - 1) = yy_temp;
    }

    /* Signal-and-Delayed Envelope */
    // stacking L_b+M_b elements in reverse order
    cx_fcolvec y_vec_arma23(L_b + M_b, fill::zeros);
    for (int ii = L_b + M_b - 1; ii >= 0; ii--)
        y_vec_arma23(ii) = in[item - ii];

    // L_b signal elements
    cx_fcolvec y_vec_arma2 = y_vec_arma23.rows(0, L_b - 1);

    // store abs() of y_vec_arma23
    cx_fcolvec abs_y_vec_arma23(size(y_vec_arma23), fill::zeros);
    abs_y_vec_arma23.set_real(abs(y_vec_arma23));

    for (int mm = 1; mm < M_b + 1; mm++) {
        // stacking L_b delayed signal-envelope elements
        cx_fcolvec abs_y_vec_arma3 = abs_y_vec_arma23.rows(mm, L_b + mm - 1);

        cx_fcolvec yy_temp;
        yy_temp = y_vec_arma2 % abs_y_vec_arma3;
        GMP_vector.rows(K_a * L_a + (mm - 1) * K_b * L_b,
                        K_a * L_a + (mm - 1) * K_b * L_b + L_b - 1) = yy_temp;

        for (int kk = 2; kk < K_b + 1; kk++) {
            // perform element-wise product using the overloaded % operator
            yy_temp = yy_temp % abs_y_vec_arma3;

            GMP_vector.rows(K_a * L_a + (mm - 1) * K_b * L_b + (kk - 1) * L_b,
                            K_a * L_a + (mm - 1) * K_b * L_b + kk * L_b - 1) = yy_temp;
        }
    }
}
int stream_to_gmp_vector_impl::work(int noutput_items,
                                    gr_vector_const_void_star& input_items,
                                    gr_vector_void_star& output_items)
{
    // const <+ITYPE+> *in = (const <+ITYPE+> *) input_items[0];
    // <+OTYPE+> *out = (<+OTYPE+> *) output_items[0];

    // Do <+signal processing+>
    for (int item = history() - 1; item < noutput_items + history() - 1; item++) {
        cx_fcolvec GMP_vector(((gr_complex*)output_items[0]) +
                                  (item - history() + 1) * (M),
                              M,
                              COPY_MEM,
                              FIX_SIZE);
        gen_GMPvector(
            (const gr_complex*)input_items[0], item, K_a, L_a, K_b, M_b, L_b, GMP_vector);
    }

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace dpd */
} /* namespace gr */
