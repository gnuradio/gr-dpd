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
 * \brief <+description of block+>
 * \ingroup dpd
 *
 */
class DPD_API predistorter_training : virtual public gr::sync_block
{
public:
    typedef boost::shared_ptr<predistorter_training> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of dpd::predistorter_training.
     *
     * To avoid accidental use of raw pointers, dpd::predistorter_training's
     * constructor is in a private implementation
     * class. dpd::predistorter_training::make is the public interface for
     * creating new instances.
     */
    static sptr make(const std::vector<int>& dpd_params);
};

} // namespace dpd
} // namespace gr

#endif /* INCLUDED_DPD_PREDISTORTER_TRAINING_H */
