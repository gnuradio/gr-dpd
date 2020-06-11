/* -*- c++ -*- */
/*
 * Copyright 2020 gr-dpd author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DPD_STREAM_TO_MESSAGE_H
#define INCLUDED_DPD_STREAM_TO_MESSAGE_H

#include <gnuradio/sync_block.h>
#include <dpd/api.h>

namespace gr {
namespace dpd {

/*!
 * \brief <+description of block+>
 * \ingroup dpd
 *
 */
class DPD_API stream_to_message : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<stream_to_message> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of dpd::stream_to_message.
     *
     * To avoid accidental use of raw pointers, dpd::stream_to_message's
     * constructor is in a private implementation
     * class. dpd::stream_to_message::make is the public interface for
     * creating new instances.
     */
    static sptr make();
};

} // namespace dpd
} // namespace gr

#endif /* INCLUDED_DPD_STREAM_TO_MESSAGE_H */
