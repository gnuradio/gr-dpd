/* -*- c++ -*- */
/*
 * Copyright 2020 gr-dpd author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "GMP_model_PA_impl.h"
#include <gnuradio/io_signature.h>
#include <math.h>
#include <algorithm>
#include <armadillo>

using std::vector;
using namespace arma;

cx_fmat coeff1 = {
    { { 0.9295, -0.0001 },
      { 0.2939, 0.0005 },
      { -0.1270, 0.0034 },
      { 0.0741, 0.0018 } }, // 1st order coeffs
    { { 0.8295, -0.0001 }, { 0.1939, 0.0005 }, { -0.0270, 0.0014 }, { 0.0341, 0.0008 } },
    { { 0.1419, -0.0008 },
      { -0.0735, 0.0833 },
      { -0.0535, 0.0004 },
      { 0.0908, -0.0473 } }, // 3rd order coeffs
    { { 0.0719, -0.0004 },
      { -0.0735, 0.0333 },
      { -0.0535, 0.0002 },
      { 0.0908, -0.0273 } },
    { { 0.0084, -0.0569 },
      { -0.4610, 0.0274 },
      { -0.3011, -0.1403 },
      { -0.0623, -0.0269 } }, // 5th order coeffs
    { { 0.0044, -0.0369 },
      { -0.2610, 0.0174 },
      { -0.1511, -0.0703 },
      { -0.0323, -0.0169 } },
    { { 0.1774, 0.0265 }, { 0.0848, 0.0613 }, { -0.0362, -0.0307 }, { 0.0415, 0.0429 } }
}; // 7th order coeffs

cx_fmat coef = { { { 0.2000, 0.0500 }, { 0.1000, -0.0050 }, { 0.0100, 0.0010 } },
                 { { 0.2000, -0.0500 }, { 0.1000, 0.0050 }, { 0.0100, -0.0010 } },
                 { { 0.2000, 0.0500 }, { 0.1000, -0.0050 }, { 0.0100, 0.0010 } } };
cx_fcube coeff2(3, 3, 3);


namespace gr {
namespace dpd {

GMP_model_PA::sptr GMP_model_PA::make(int model_param1,
                                      int model_param2,
                                      int model_param3,
                                      int model_param4,
                                      int model_param5,
                                      int mode)
{
    return gnuradio::get_initial_sptr(new GMP_model_PA_impl(
        model_param1, model_param2, model_param3, model_param4, model_param5, mode));
}


/*
 * The private constructor
 */
GMP_model_PA_impl::GMP_model_PA_impl(int model_param1,
                                     int model_param2,
                                     int model_param3,
                                     int model_param4,
                                     int model_param5,
                                     int mode)
    : gr::sync_block("GMP_model_PA",
                     gr::io_signature::make(1, 1, sizeof(gr_complex)),
                     gr::io_signature::make(1, 1, sizeof(gr_complex))),
      K_a(model_param1),
      L_a(model_param2),
      K_b(model_param3),
      M_b(model_param4),
      L_b(model_param5),
      M(model_param1 * model_param2 + model_param3 * model_param4 * model_param5),
      Mode_vl(mode)
{
    set_history(std::max(L_a, M_b + L_b));
    coeff2.slice(0) = coef;
    coeff2.slice(1) = coef;
    coeff2.slice(2) = coef;
}

/*
 * Our virtual destructor.
 */
GMP_model_PA_impl::~GMP_model_PA_impl() {}

void GMP_model_PA_impl::gen_GMP_vector(const gr_complex* const in,
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
int GMP_model_PA_impl::work(int noutput_items,
                            gr_vector_const_void_star& input_items,
                            gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];

    // Do <+signal processing+>
    for (int item = history() - 1; item < noutput_items + history() - 1; item++) {
        out[item - history() + 1] = { 0, 0 };
        // Converting stream to MP vector
        cx_fcolvec GMP_vector(K_a * L_a + K_b * L_b * M_b);
        gen_GMP_vector((const gr_complex*)in, item, K_a, L_a, K_b, M_b, L_b, GMP_vector);
        for (int K = 0; K < K_a; K++) {
            int L_st = (K * L_a);
            int L_en = ((K + 1) * L_a);
            // Include terms in output according to Mode of Operation value
            if((K % 2) == 0 && Mode_vl == 1)
              continue;
            else if((K % 2) && Mode_vl == 2)
              continue;
            for (int L = L_st; L < L_en; L++) {
                gr_complex a = GMP_vector(L);
                gr_complex b = coeff1(K, (L - L_st));
                out[item - history() + 1] += (a * b);
            }
        }
        // gr_complex a = GMP_vector((K_a * L_a));
        // gr_complex b = coeff2(2, 2, 2);
        // out[item - history() + 1] += (a * b);
        for (int m = 0; m < M_b; m++) {
            for (int k = 0; k < K_b; k++) {
                // Include terms in output according to Mode of Operation value
                if((k % 2) == 0 && Mode_vl == 2)
                  continue;
                else if((k % 2) && Mode_vl == 1)
                  continue; 
                int L_st = (m * L_b * K_b) + (K_a * L_a) + k * L_a;
                int L_en = (m * L_b * K_b) + (K_a * L_a) + (k + 1) * L_a;
                for (int l = L_st; l < L_en; l++) {
                    gr_complex a = GMP_vector(l);
                    gr_complex b = coeff2(k, m, l - L_st);
                    out[item - history() + 1] += (a * b);
                }
            }
        }
    }
    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace dpd */
} /* namespace gr */
