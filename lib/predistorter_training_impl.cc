/* -*- c++ -*- */
/*
 * Copyright 2020 Alekh Gupta
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "predistorter_training_impl.h"
#include <gnuradio/io_signature.h>
#include <armadillo>
#include <ctime>
#include <string>
#define COPY_MEM false // Do not copy vectors into separate memory
#define FIX_SIZE true  // Keep dimensions of vectors constant

using std::vector;
using namespace arma;

namespace gr {
namespace dpd {

predistorter_training::sptr
predistorter_training::make(const std::vector<int>& dpd_params,
                            std::string mode,
                            const std::vector<gr_complex>& taps)
{
    return gnuradio::get_initial_sptr(
        new predistorter_training_impl(dpd_params, mode, taps));
}


/*
 * The private constructor
 */
predistorter_training_impl::predistorter_training_impl(
    const std::vector<int>& dpd_params,
    std::string mode,
    const std::vector<gr_complex>& taps)
    : gr::sync_block("predistorter_training",
                     gr::io_signature::make(1, 1, sizeof(gr_complex)),
                     gr::io_signature::make(1, 2, sizeof(gr_complex))),
      K_a(dpd_params[0]),
      L_a(dpd_params[1]),
      K_b(dpd_params[2]),
      M_b(dpd_params[3]),
      L_b(dpd_params[4]),
      d_M(dpd_params[0] * dpd_params[1] + dpd_params[2] * dpd_params[3] * dpd_params[4]),
      d_mode(mode)
{
    d_update_predistorter_training = true;
    d_predistorter_training_colvec.set_size(d_M);
    d_predistorter_training_colvec.zeros();
    d_predistorter_training_colvec(0) = gr_complex(1.0, 0.0);
    set_history(std::max(L_a, M_b + L_b));
    // setup output message port for
    // sending predistorted PA input to the postdistorter
    // message_port_register_out(pmt::mp("PA_input"));


    if (mode == "static")
        for (int i = 0; i < taps.size(); i++) {
            d_predistorter_training_colvec(i) = taps[i];
        }
    // setup input message port
    message_port_register_in(pmt::mp("taps"));
    set_msg_handler(pmt::mp("taps"),
                    boost::bind(&predistorter_training_impl::get_taps, this, _1));
}

/*
 * Our virtual destructor.
 */
predistorter_training_impl::~predistorter_training_impl() {}

void predistorter_training_impl::get_taps(pmt::pmt_t P)
{
    d_update_predistorter_training = true;

    // extract predistorter_training weight vector from the message
    for (int i = 0; i < pmt::length(P); i++)
        d_predistorter_training_colvec(i) = pmt::c64vector_ref(P, i);
}
void predistorter_training_impl::gen_GMPvector(const gr_complex* const in,
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

    if(K_b == 0)
        return;

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
int predistorter_training_impl::work(int noutput_items,
                                     gr_vector_const_void_star& input_items,
                                     gr_vector_void_star& output_items)
{
    gr_complex* out = (gr_complex*)output_items[0];
    gr_complex* flag = (gr_complex*)output_items[1];

    predistorter_training_colvec = d_predistorter_training_colvec;
    
    // Do <+signal processing+>
    for (int item = history() - 1; item < noutput_items + history() - 1; item++) {
        
        update_predistorter_training = d_update_predistorter_training;
        // get PA input which has been arranged in a GMP vector format
        // for predistortion
        cx_fcolvec GMP_vector(d_M);
        gen_GMPvector(
            (const gr_complex*)input_items[0], item, K_a, L_a, K_b, M_b, L_b, GMP_vector);
        cx_fmat yy_cx_rowvec = GMP_vector.st();

        // apply predistortion and send the PA input to postdistorter
        out[item - history() + 1] = as_scalar(
            conv_to<cx_fmat>::from(yy_cx_rowvec * predistorter_training_colvec));

        // Flag output only if mode of operation is 'static' 
        if(d_mode == "static")
        {
        	continue;
        }

        // Setting flag output according to whether 'taps' are recieved 
        // or not
        if(update_predistorter_training || item == history() - 1)
        {
            flag[item - history() + 1] = gr_complex(1.0, 0.0);
        }
        else
            flag[item - history() + 1] = gr_complex(0.0, 0.0);
        d_update_predistorter_training = false;
    }
    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace dpd */
} /* namespace gr */
