/* -*- c++ -*- */
/*
 * Copyright 2020 Alekh Gupta
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DPD_PREDISTORTER_TRAINING_H
#define INCLUDED_DPD_PREDISTORTER_TRAINING_H

#include <gnuradio/sync_block.h>
#include <dpd/api.h>

namespace gr {
namespace dpd {

/*!
 * \brief  Performs predistortion of the input stream to be fed to
 *  the Power Amplifier (PA).
 * \ingroup dpd
 *
 * \details
 *  It has to be necessarily supplied with the parameters
 *  (as complex vector) of the behavioral model (GMP) used for
 *  estimation of the PA model coefficients or predistortion
 *  coefficients (inverse of PA model estimated).
 *
 *  It can operate in both Static and Training modes of predistortion.
 *
 *  * In static mode, predistortion coefficients are given by user
 *  as complex_vector parameter.
 *  * In training mode, it is necessary to connect 'taps' input
 *  message-port to a postdistorter block passing message 'taps'
 *  after adaptive-estimation.
 *
 *  It multiplies GMP row vector for each input complex value with
 *  predistorter coefficients column vector (of same order) to give a
 *  predistorted input to PA.
 *
 *  Mathematically, Output[m] = Input[m] * taps
 *  (where Input[m] is input GMP vector(row) and 'taps' is weight vector(col)).
 */
class DPD_API predistorter_training : virtual public gr::sync_block
{
public:
    typedef boost::shared_ptr<predistorter_training> sptr;

    /*!
     * \brief Make a predistorter_training block
     *
     * \param dpd_params The (K_a, L_a, K_b, L_b, M_b) int_vector denoting the GMP model
     * parameters or DPD parameters. Used to determine the order of GMP vector generated
     * for each input for its predistortion. \param mode Modes of Operation, i.e.,
     * Training or Static. \param taps Predistortion coefficients or taps as
     * complex_vector (Optional ,i.e., Parameter only in static mode).
     *
     */
    static sptr make(const std::vector<int>& dpd_params,
                     std::string mode,
                     const std::vector<gr_complex>& taps);
};

} // namespace dpd
} // namespace gr

#endif /* INCLUDED_DPD_PREDISTORTER_TRAINING_H */
