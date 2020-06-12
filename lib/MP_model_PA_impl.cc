/* -*- c++ -*- */
/*
 * Copyright 2020 gr-dpd author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "MP_model_PA_impl.h"
#include <gnuradio/io_signature.h>
#include <math.h>
#include <algorithm>
#include <armadillo>

using std::vector;
using namespace arma;

cx_fmat coeff = {
    { { 0.9295, -0.0001 },
      { 0.2939, 0.0005 },
      { -0.1270, 0.0034 },
      { 0.0741, 0.0018 } }, // 1st order coeffs
    { { 0.9295, -0.0001 }, { 0.2939, 0.0005 }, { -0.1270, 0.0034 }, { 0.0741, 0.0018 } },
    { { 0.1419, -0.0008 },
      { -0.0735, 0.0833 },
      { -0.0535, 0.0004 },
      { 0.0908, -0.0473 } }, // 3rd order coeffs
    { { 0.1419, -0.0008 },
      { -0.0735, 0.0833 },
      { -0.0535, 0.0004 },
      { 0.0908, -0.0473 } },
    { { 0.0084, -0.0569 },
      { -0.4610, 0.0274 },
      { -0.3011, -0.1403 },
      { -0.0623, -0.0269 } }, // 5th order coeffs
    { { 0.0084, -0.0569 },
      { -0.4610, 0.0274 },
      { -0.3011, -0.1403 },
      { -0.0623, -0.0269 } },
    { { 0.1774, 0.0265 }, { 0.0848, 0.0613 }, { -0.0362, -0.0307 }, { 0.0415, 0.0429 } }
}; // 7th order coeffs

namespace gr {
namespace dpd {

MP_model_PA::sptr MP_model_PA::make(int Order, int Mem_Depth, int Mode)
{
    return gnuradio::get_initial_sptr(new MP_model_PA_impl(Order, Mem_Depth, Mode));
}


/*
 * The private constructor
 */
MP_model_PA_impl::MP_model_PA_impl(int Order, int Mem_Depth, int Mode)
    : gr::sync_block("MP_model_PA",
                     gr::io_signature::make(1, 1, sizeof(gr_complex)),
                     gr::io_signature::make(1, 1, sizeof(gr_complex))),
      K_a(Order),    // Max. Order limited to 7
      L_a(Mem_Depth), // Max no. of taps or memory depth limited to 4
      Mode_val(Mode)
{
    set_history(L_a);
}

/*
 * Our virtual destructor.
 */
MP_model_PA_impl::~MP_model_PA_impl() {}

void MP_model_PA_impl::gen_MP_vector(
    const gr_complex* in, int item, int K_a, int L_a, cx_fcolvec& MP_vector)
{
    /* Signal-and-Aligned Envelope */
    // stacking L_a elements in reverse order
    cx_fcolvec y_vec_arma1(L_a, fill::zeros);
    for (int ii = L_a - 1; ii >= 0; ii--) {
        y_vec_arma1(ii) = in[item - ii];
    }
    MP_vector.rows(0, L_a - 1) = y_vec_arma1;

    // store abs() of y_vec_arma1
    cx_fcolvec abs_y_vec_arma1(size(y_vec_arma1), fill::zeros);
    abs_y_vec_arma1.set_real(abs(y_vec_arma1));

    cx_fcolvec yy_temp;
    yy_temp = y_vec_arma1 % abs_y_vec_arma1;
    if (K_a > 1)
        MP_vector.rows(L_a, 2 * L_a - 1) = yy_temp;

    for (int kk = 2; kk < K_a; kk++) {
        // perform element-wise product using the overloaded % operator
        yy_temp = yy_temp % abs_y_vec_arma1;

        MP_vector.rows(kk * L_a, (kk + 1) * L_a - 1) = yy_temp;
    }
}

int MP_model_PA_impl::work(int noutput_items,
                           gr_vector_const_void_star& input_items,
                           gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];

    // Do <+signal processing+>
    for (int item = history() - 1; item < noutput_items + history() - 1; item++) {
        out[item - history() + 1] = { 0, 0 };
        // Converting stream to MP vector
        cx_fcolvec MP_vector(K_a * L_a);
        gen_MP_vector((const gr_complex*)in, item, K_a, L_a, MP_vector);
        for (int K = 0; K < K_a; K++) {
            int L_st = (K * L_a);
            int L_en = ((K + 1) * L_a);
            if((K % 2) == 0 && Mode_val == 1)
            	continue;
            else if((K % 2) && Mode_val == 2)
            	continue;
            for (int L = L_st; L < L_en; L++) {
                gr_complex a = MP_vector(L);
                gr_complex b = coeff(K, (L - L_st));
                out[item - history() + 1] += (a * b);
            }
        }
    }
    // Tell runtime system how many output items we produced.
    return noutput_items;
}
} // namespace dpd
} // namespace gr
  /* namespace gr */
