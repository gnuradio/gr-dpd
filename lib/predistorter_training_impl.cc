/* -*- c++ -*- */
/*
 * Copyright 2020 gr-dpd author.
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

predistorter_training::sptr predistorter_training::make(int M)
{
    return gnuradio::get_initial_sptr(new predistorter_training_impl(M));
}


/*
 * The private constructor
 */
predistorter_training_impl::predistorter_training_impl(int M)
    : gr::sync_block("predistorter_training",
                     gr::io_signature::make(1, 1, M * sizeof(gr_complex)),
                     gr::io_signature::make(1, 1, sizeof(gr_complex))),
      d_M(M)
{
    d_update_predistorter_training = true;
    d_predistorter_training_colvec.set_size(d_M);
    d_predistorter_training_colvec.zeros();
    d_predistorter_training_colvec(0) = gr_complex(1.0, 0.0);

    // setup output message port for
    // sending predistorted PA input to the postdistorter
    message_port_register_out(pmt::mp("PA_input"));

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

int predistorter_training_impl::work(int noutput_items,
                                     gr_vector_const_void_star& input_items,
                                     gr_vector_void_star& output_items)
{
    gr_complex* out = (gr_complex*)output_items[0];

    // Do <+signal processing+>
    predistorter_training_colvec = d_predistorter_training_colvec;
    update_predistorter_training = d_update_predistorter_training;
    for (int item = 0; item < noutput_items; item++) {
        // get PA input which has been arranged in a GMP vector format
        // for predistortion
        cx_fmat yy_cx_rowvec(
            ((gr_complex*)input_items[0]) + item * d_M, 1, d_M, COPY_MEM, FIX_SIZE);

        // apply predistortion and send the PA input to postdistorter
        out[item] = as_scalar(
            conv_to<cx_fmat>::from(yy_cx_rowvec * predistorter_training_colvec));
        pmt::pmt_t P_complex_PA_input = pmt::from_complex(out[item]);
        message_port_pub(pmt::mp("PA_input"), P_complex_PA_input);
    }
    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace dpd */
} /* namespace gr */
