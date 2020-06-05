/* -*- c++ -*- */
/*
 * Copyright 2020 gr-dpd author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DPD_PREDISTORTER_TRAINING_H
#define INCLUDED_DPD_PREDISTORTER_TRAINING_H

#include <dpd/api.h>
#include <gnuradio/sync_block.h>

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
      typedef std::shared_ptr<predistorter_training> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of dpd::predistorter_training.
       *
       * To avoid accidental use of raw pointers, dpd::predistorter_training's
       * constructor is in a private implementation
       * class. dpd::predistorter_training::make is the public interface for
       * creating new instances.
       */
      static sptr make(int M);
    };

  } // namespace dpd
} // namespace gr

#endif /* INCLUDED_DPD_PREDISTORTER_TRAINING_H */

