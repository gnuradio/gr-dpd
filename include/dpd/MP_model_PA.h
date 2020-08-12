/* -*- c++ -*- */
/*
 * Copyright 2020 Alekh Gupta
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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

#ifndef INCLUDED_DPD_MP_MODEL_PA_H
#define INCLUDED_DPD_MP_MODEL_PA_H

#include <gnuradio/sync_block.h>
#include <dpd/api.h>

namespace gr {
namespace dpd {

/*!
 * \brief <+description of block+>
 * \ingroup dpd
 *
 */
class DPD_API MP_model_PA : virtual public gr::sync_block
{
public:
    typedef boost::shared_ptr<MP_model_PA> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of dpd::MP_model_PA.
     *
     * To avoid accidental use of raw pointers, dpd::MP_model_PA's
     * constructor is in a private implementation
     * class. dpd::MP_model_PA::make is the public interface for
     * creating new instances.
     */
    static sptr make(int Order,
                     int Mem_Depth,
                     std::string Mode,
                     const std::vector<gr_complex>& Coeff);
};

} // namespace dpd
} // namespace gr

#endif /* INCLUDED_DPD_MP_MODEL_PA_H */
