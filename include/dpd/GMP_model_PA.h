/* -*- c++ -*- */
/*
 * Copyright 2020 Alekh Gupta
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DPD_GMP_MODEL_PA_H
#define INCLUDED_DPD_GMP_MODEL_PA_H

#include <gnuradio/sync_block.h>
#include <dpd/api.h>

namespace gr {
namespace dpd {

/*!
 * \brief <+description of block+>
 * \ingroup dpd
 *
 */
class DPD_API GMP_model_PA : virtual public gr::sync_block
{
public:
    typedef boost::shared_ptr<GMP_model_PA> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of dpd::GMP_model_PA.
     *
     * To avoid accidental use of raw pointers, dpd::GMP_model_PA's
     * constructor is in a private implementation
     * class. dpd::GMP_model_PA::make is the public interface for
     * creating new instances.
     */
    static sptr make(int model_param1,
                     int model_param2,
                     int model_param3,
                     int model_param4,
                     int model_param5,
                     std::string mode,
                     const std::vector <gr_complex> &coeff1,
                     const std::vector <gr_complex> &coeff2);
};

} // namespace dpd
} // namespace gr

#endif /* INCLUDED_DPD_GMP_MODEL_PA_H */
