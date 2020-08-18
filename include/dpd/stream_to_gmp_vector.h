/* -*- c++ -*- */
/*
 * Copyright 2020 Alekh Gupta
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
 * \brief Converts a stream of single complex data items into corresponding
 *  GMP vectors based on GMP model parameters using input items history.
 * \ingroup dpd
 *
 * \details
 *  Both Input and Output items are complex.
 *  GMP vectors are shift-structured vectors.
 *  Input is a single item but output is of order, say M (length of GMP vector
 *  based on parameters).
 *
 */
class DPD_API stream_to_gmp_vector : virtual public gr::sync_block
{
public:
    typedef boost::shared_ptr<stream_to_gmp_vector> sptr;

    /*!
     * \brief Make stream_to_gmp_vector block
     *
     * \param dpd_params The (K_a, L_a, K_b, L_b, M_b) int_vector denoting
     *  the GMP model parameters used to determine the order and generate
     *  the GMP vector corresponding to each input data item.
     *
     */
    static sptr make(const std::vector<int>& dpd_params);
};

} // namespace dpd
} // namespace gr

#endif /* INCLUDED_DPD_STREAM_TO_GMP_VECTOR_H */
