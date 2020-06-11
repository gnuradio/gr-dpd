/* -*- c++ -*- */
/*
 * Copyright 2020 gr-dpd author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "stream_to_message_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace dpd {

stream_to_message::sptr stream_to_message::make()
{
    return gnuradio::get_initial_sptr(new stream_to_message_impl());
}


/*
 * The private constructor
 */
stream_to_message_impl::stream_to_message_impl()
    : gr::sync_block("stream_to_message",
                     gr::io_signature::make(1, 1, sizeof(gr_complex)),
                     gr::io_signature::make(0, 0, 0))
{
    message_port_register_out(pmt::mp("samples"));
}

/*
 * Our virtual destructor.
 */
stream_to_message_impl::~stream_to_message_impl() {}

int stream_to_message_impl::work(int noutput_items,
                                 gr_vector_const_void_star& input_items,
                                 gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];

    // Do <+signal processing+>

    for (int i = 0; i < noutput_items; i++) {
        pmt::pmt_t P_complex_samples = pmt::from_complex(in[i]);
        message_port_pub(pmt::mp("samples"), P_complex_samples);
    }

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace dpd */
} /* namespace gr */
