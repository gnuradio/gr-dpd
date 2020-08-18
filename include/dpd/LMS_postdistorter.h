/* -*- c++ -*- */
/*
 * Copyright 2020 gr-dpd author.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_DPD_LMS_POSTDISTORTER_H
#define INCLUDED_DPD_LMS_POSTDISTORTER_H

#include <gnuradio/sync_block.h>
#include <dpd/api.h>

namespace gr {
namespace dpd {

/*!
 * \brief LMS based Algorithm implemented to estimate the coefficients of
 *  the behaviorial model (GMP) of the Power Amplifier and thus
 *  predistorter taps.
 * \ingroup dpd
 *
 * \details
 *  It estimates the coefficients based on the PA_output and PA_input.
 *  It uses the LMS (Least Mean Squares) Algorithm with two methods options,
 *  namely, Newton based and EMA based methods.
 *
 *  It has two input ports, one for the PA_output (gain phase calibrated)
 *  and other for the PA_input (or predistorter output).
 *
 *  The coefficients estimated are passed as messages through message
 *  output port 'taps'.
 *  Both Input ports are only for Complex Data Type.
 */
class DPD_API LMS_postdistorter : virtual public gr::sync_block
{
public:
    typedef boost::shared_ptr<LMS_postdistorter> sptr;

    /*!
     * \brief Make LMS_postdistorter
     *
     * \param dpd_params The (K_a, L_a, K_b, L_b, M_b) int_vector denoting
     *  the GMP model parameters used for predistorter 'taps' estimation.
     *  Total No. of coefficients = ((K_a * L_a) + (K_b * M_b * L_b))
     * \param iter_limit Iteration limit or Max. number of iterations of training
     *  to be performed for predistorter DPD coefficients estimation.
     * \param method Method of LMS algorithm used for coefficients estimation,
     *  i.e., Newton or EMA based method.
     *
     */
    static sptr
    make(const std::vector<int>& dpd_params, int iter_limit, std::string method);
};

} // namespace dpd
} // namespace gr

#endif /* INCLUDED_DPD_LMS_POSTDISTORTER_H */
