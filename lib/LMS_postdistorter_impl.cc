/* -*- c++ -*- */
/*
 * Copyright 2020 gr-dpd author.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#define ARMA_DONT_PRINT_ERRORS
#include "LMS_postdistorter_impl.h"
#include <gnuradio/io_signature.h>
#include <armadillo>
#include <string>

#define COPY_MEM false // Do not copy matrices into separate memory
#define FIX_SIZE true  // Keep dimensions of matrices constant

using namespace std;
using std::string;
using namespace arma;
namespace gr {
namespace dpd {

LMS_postdistorter::sptr LMS_postdistorter::make(const std::vector<int>& dpd_params,
                                                int iter_limit,
                                                std::string method,
                                                gr_complexd learning_rate)
{
    return gnuradio::get_initial_sptr(
        new LMS_postdistorter_impl(dpd_params, iter_limit, method, learning_rate));
}


/*
 * The private constructor
 */
LMS_postdistorter_impl::LMS_postdistorter_impl(const std::vector<int>& dpd_params,
                                               int iter_limit,
                                               std::string method,
                                               gr_complexd learning_rate)
    : gr::sync_block("LMS_postdistorter",
                     gr::io_signature::make(2, 2, sizeof(gr_complex)),
                     gr::io_signature::make(0, 0, 0)),
      d_dpd_params(dpd_params),
      K_a(d_dpd_params[0]),
      L_a(d_dpd_params[1]),
      K_b(d_dpd_params[2]),
      M_b(d_dpd_params[3]),
      L_b(d_dpd_params[4]),
      M(dpd_params[0] * dpd_params[1] + dpd_params[2] * dpd_params[3] * dpd_params[4]),
      d_iter_limit(iter_limit),
      str(method),
      learning_rate(learning_rate)
{
    // set_history(std::max(L_a, L_b + M_b));
    iteration = 1;

    for (int ii = 0; ii < sreg_len; ii++)
        sreg[ii] = 0.0;

    // setup output message port
    message_port_register_out(pmt::mp("taps"));
    // lambda = 1 - pow(2, 1 - k);

    // taps or weight-vector
    w_iMinus1.set_size(M, 1);
    w_iMinus1 = zeros<cx_mat>(M, 1);
    w_iMinus1(0, 0) = gr_complex(1.0, 0.0);
}

/*
 * Our virtual destructor.
 */
LMS_postdistorter_impl::~LMS_postdistorter_impl() {}

cx_mat LMS_postdistorter_impl::ls_estimation(cx_mat A, gr_complexd y)
{
    cx_mat lambda(size(A * (A.t())));
    lambda.fill(0.001);
    // lambda(0, 0) = 0.001;
    cx_mat yy(1, 1);
    yy(0, 0) = y;
    cx_mat den = A * yy;
    cx_mat num = A * (A.t());
    num = num + lambda * eye<cx_mat>(size(A * (A.t())));
    cx_mat ls_result =
        solve((A * (A.t()) + lambda * eye<cx_mat>(size(A * (A.t())))), A * yy);
    // cx_mat ls_result(size(w_iMinus1), fill::zeros);
    return ls_result;
}

void LMS_postdistorter_impl::gen_GMPvector(const gr_complexd* const in,
                                           int item,
                                           int K_a,
                                           int L_a,
                                           int K_b,
                                           int M_b,
                                           int L_b,
                                           cx_colvec& GMP_vector)
{
    /* Signal-and-Aligned Envelope */
    // stacking L_a elements in reverse order
    cx_colvec y_vec_arma1(L_a, fill::zeros);
    for (int ii = L_a - 1; ii >= 0; ii--)
        y_vec_arma1(ii) = in[item - ii];
    GMP_vector.rows(0, L_a - 1) = y_vec_arma1;

    // store abs() of y_vec_arma1
    cx_colvec abs_y_vec_arma1(size(y_vec_arma1), fill::zeros);
    abs_y_vec_arma1.set_real(abs(y_vec_arma1));

    cx_colvec yy_temp;
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
    cx_colvec y_vec_arma23(L_b + M_b, fill::zeros);
    for (int ii = L_b + M_b - 1; ii >= 0; ii--)
        y_vec_arma23(ii) = in[item - ii];

    // L_b signal elements
    cx_colvec y_vec_arma2 = y_vec_arma23.rows(0, L_b - 1);

    // store abs() of y_vec_arma23
    cx_colvec abs_y_vec_arma23(size(y_vec_arma23), fill::zeros);
    abs_y_vec_arma23.set_real(abs(y_vec_arma23));

    for (int mm = 1; mm < M_b + 1; mm++) {
        // stacking L_b delayed signal-envelope elements
        cx_colvec abs_y_vec_arma3 = abs_y_vec_arma23.rows(mm, L_b + mm - 1);

        cx_colvec yy_temp;
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
int LMS_postdistorter_impl::work(int noutput_items,
                                 gr_vector_const_void_star& input_items,
                                 gr_vector_void_star& output_items)
{
    const gr_complex* in1 = (const gr_complex*)input_items[0];
    const gr_complex* in2 = (const gr_complex*)input_items[1];

    // Do <+signal processing+>
    for (int item = 0; item < noutput_items; item++) {

        // get number of samples consumed since the beginning of time by this block
        // from port 0
        const uint64_t nread = this->nitems_read(0);
        if (iteration == d_iter_limit) {
            taps = conv_to<vector<gr_complexd>>::from(w_iMinus1);
            pmt::pmt_t P_c32vector_taps = pmt::init_c64vector(M, taps);
            message_port_pub(pmt::mp("taps"), P_c32vector_taps);
            continue;
        }
        pa_input = in2[item];
        sreg[49] = in1[item];

        // extracting the PA output and arranging into a shift-structured GMP vector
        cx_colvec GMP_vector(M);
        gen_GMPvector(ptr_sreg, 49, K_a, L_a, K_b, M_b, L_b, GMP_vector);

        cx_mat pa_output = GMP_vector;
        cx_mat pa_output_t = GMP_vector.t();

        for (int ii = 1; ii < sreg_len; ii++)
            sreg[ii - 1] = sreg[ii];

        cx_mat l_rate(1, 1, fill::zeros);
        l_rate(0, 0) = learning_rate;
        cx_mat l_rate_1_minus(1, 1, fill::zeros);
        l_rate_1_minus(0, 0) = gr_complexd(1.0, 0.0) - learning_rate;
        
        if (str == "newton") {
            error = pa_input - as_scalar(pa_output_t * w_iMinus1);
            cx_mat ls_result = ls_estimation(pa_output, error);
            w_iMinus1 = w_iMinus1 + (ls_result * l_rate);
        }
        if (str == "ema") {
            cx_mat ls_result = ls_estimation(pa_output, pa_input);
            w_iMinus1 = (w_iMinus1 * l_rate_1_minus) + (ls_result * l_rate);
        }
        // send weight-vector to predistorter block in a message
        taps = conv_to<vector<gr_complexd>>::from(w_iMinus1);
        pmt::pmt_t P_c32vector_taps = pmt::init_c64vector(M, taps);
        message_port_pub(pmt::mp("taps"), P_c32vector_taps);

        iteration++;
    }
    // Tell runtime system how many output items we produced.
    return noutput_items;
}
} /* namespace dpd */
} /* namespace gr */
