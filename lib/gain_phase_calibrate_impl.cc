/* -*- c++ -*- */
/*
 * Copyright 2020 Alekh Gupta
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gain_phase_calibrate_impl.h"
#include <gnuradio/io_signature.h>
namespace gr {
namespace dpd {

gain_phase_calibrate::sptr gain_phase_calibrate::make()
{
    return gnuradio::get_initial_sptr(new gain_phase_calibrate_impl());
}


/*
 * The private constructor
 */
gain_phase_calibrate_impl::gain_phase_calibrate_impl()
    : gr::block("gain_phase_calibrate",
                gr::io_signature::make(2, 2, sizeof(gr_complex)),
                gr::io_signature::make(1, 1, sizeof(gr_complex)))
{
    previous_cfactor = gr_complex(0.0, 0.0);
    d_reference_acquired = false;
    d_sample = gr_complex(0.0, 0.0);

    // Setup input port
    // message_port_register_in(pmt::mp("samples"));
    // set_msg_handler(pmt::mp("samples"),
    //                 boost::bind(&gain_phase_calibrate_impl::set_reference, this, _1));
}
/*
 * Our virtual destructor.
 */
gain_phase_calibrate_impl::~gain_phase_calibrate_impl() {}

// void gain_phase_calibrate_impl::set_reference(pmt::pmt_t P)
// {
//     d_reference_acquired = true;

//     // extract reference samples from the message
//     d_sample = pmt::to_complex(P);
// }
bool gain_phase_calibrate_impl::almost_equals_zero(double a, int num_digits)
{
    // identify the first few significant digits
    int na = floor(fabs(a) * pow(10.0, num_digits));

    if (na == 0)
        return true;

    return false;
}
int gain_phase_calibrate_impl::general_work(int noutput_items,
                                            gr_vector_int& ninput_items_,
                                            gr_vector_const_void_star& input_items,
                                            gr_vector_void_star& output_items)
{
    const gr_complex* in1 = (const gr_complex*)input_items[0];
    const gr_complex* in2 = (const gr_complex*)input_items[1];
    gr_complex* out = (gr_complex*)output_items[0];


    ninput_items = std::min(ninput_items_[0], noutput_items);
    item = 0;
    gr_complex cfactor_avg_sum = gr_complex(0.0, 0.0);
    while (item < ninput_items) {
        // reference_acquired = d_reference_acquired;
        // sample = d_sample;
        sample = in2[item];
        current_cfactor = sample / in1[item];
        //if (reference_acquired) {
            if(previous_cfactor != gr_complex(0.0, 0.0))
            {
            	cfactor_avg_sum = cfactor_avg_sum + current_cfactor;
                cfactor = cfactor_avg_sum/gr_complex(item + 1.0);
            }
            else
                cfactor = current_cfactor;

            // cfactor = gr_complex(0.5, 0.0) * (previous_cfactor + current_cfactor);

            if (!almost_equals_zero(std::real(in1[item]), 5) &&
                !almost_equals_zero(std::imag(in1[item]), 5))
                previous_cfactor = cfactor;

            out[item] = cfactor * in1[item];
            item++;
        //}
    }

    // Do <+signal processing+>
    // Tell runtime system how many input items we consumed on
    // each input stream.
    consume_each(noutput_items);

    // Tell runtime system how many output items we produced.
    return ninput_items;
}

} /* namespace dpd */
} /* namespace gr */
