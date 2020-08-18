/* -*- c++ -*- */
/*
 * Copyright 2020 Alekh Gupta
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DPD_RLS_POSTDISTORTER_H
#define INCLUDED_DPD_RLS_POSTDISTORTER_H

#include <gnuradio/sync_block.h>
#include <dpd/api.h>

namespace gr {
namespace dpd {

/*!
 * \brief RLS based Algorithm implemented to estimate the coefficients of the
 *  behaviorial model (GMP) of the Power Amplifier and thus predistorter taps.
 * \ingroup dpd
 *
 * \details
 *  It estimates the coefficients based on the PA_output and PA_input.
 *
 *  It has two input ports, one for the PA_output (gain phase calibrated) and
 *  other for the PA_input (or predistorter output).
 *
 *  The coefficients estimated are passed as messages through message output
 *  port 'taps'.
 *  Both Input ports are only for Complex Data Type.
 *
 *  The Fast RLS Algorithm used in coefficient estimations is referenced from the
 *  publication: S. Pagadarai, R. Grover, S. J. Macmullan and A. M. Wyglinski,
 *  "Digital Predistortion of Power Amplifiers for Spectrally Agile Wireless
 *  Transmitters."
 */
class DPD_API RLS_postdistorter : virtual public gr::sync_block
{
public:
    typedef boost::shared_ptr<RLS_postdistorter> sptr;

    /*!
     * \brief Make RLS_postdistorter block
     *
     * \param dpd_params The (K_a, L_a, K_b, L_b, M_b) int_vector denoting the
     *  GMP model parameters used for predistorter coefficients estimations.
     *  Total No. of coefficients = ((K_a * L_a) + (K_b * M_b * L_b))
     * \param iter_limit Iteration limit or Max. number of iterations of training
     *  to be performed for predistorter DPD coefficients estimation.
     *
     *
     */
    static sptr make(const std::vector<int>& dpd_params, int iter_limit);
};

} // namespace dpd
} // namespace gr

#endif /* INCLUDED_DPD_RLS_POSTDISTORTER_H */
