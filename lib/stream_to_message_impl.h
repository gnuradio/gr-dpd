/* -*- c++ -*- */
/*
 * Copyright 2020 gr-dpd author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DPD_STREAM_TO_MESSAGE_IMPL_H
#define INCLUDED_DPD_STREAM_TO_MESSAGE_IMPL_H

#include <dpd/stream_to_message.h>

namespace gr {
  namespace dpd {

    class stream_to_message_impl : public stream_to_message
    {
     private:
      // Nothing to declare in this block.

     public:
      stream_to_message_impl();
      ~stream_to_message_impl();

      // Where all the action really happens
      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );
    };

  } // namespace dpd
} // namespace gr

#endif /* INCLUDED_DPD_STREAM_TO_MESSAGE_IMPL_H */

