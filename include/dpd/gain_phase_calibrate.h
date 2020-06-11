/* -*- c++ -*- */
/*
 * Copyright 2020 gr-dpd author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DPD_GAIN_PHASE_CALIBRATE_H
#define INCLUDED_DPD_GAIN_PHASE_CALIBRATE_H

#include <gnuradio/block.h>
#include <dpd/api.h>

namespace gr {
namespace dpd {

/*!
 * \brief <+description of block+>
 * \ingroup dpd
 *
 */
class DPD_API gain_phase_calibrate : virtual public gr::block
{
public:
    typedef std::shared_ptr<gain_phase_calibrate> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of dpd::gain_phase_calibrate.
     *
     * To avoid accidental use of raw pointers, dpd::gain_phase_calibrate's
     * constructor is in a private implementation
     * class. dpd::gain_phase_calibrate::make is the public interface for
     * creating new instances.
     */
    static sptr make();
};

} // namespace dpd
} // namespace gr

#endif /* INCLUDED_DPD_GAIN_PHASE_CALIBRATE_H */
