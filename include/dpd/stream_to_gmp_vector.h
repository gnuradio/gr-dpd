/* -*- c++ -*- */
/*
 * Copyright 2020 gr-dpd author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DPD_STREAM_TO_GMP_VECTOR_H
#define INCLUDED_DPD_STREAM_TO_GMP_VECTOR_H

#include <gnuradio/sync_block.h>
#include <dpd/api.h>

namespace gr {
namespace dpd {

/*!
 * \brief <+description of block+>
 * \ingroup dpd
 *
 */
class DPD_API stream_to_gmp_vector : virtual public gr::sync_block
{
public:
    typedef boost::shared_ptr<stream_to_gmp_vector> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of dpd::stream_to_gmp_vector.
     *
     * To avoid accidental use of raw pointers, dpd::stream_to_gmp_vector's
     * constructor is in a private implementation
     * class. dpd::stream_to_gmp_vector::make is the public interface for
     * creating new instances.
     */
    static sptr make(const std::vector<int>& dpd_params);
};

} // namespace dpd
} // namespace gr

#endif /* INCLUDED_DPD_STREAM_TO_GMP_VECTOR_H */
