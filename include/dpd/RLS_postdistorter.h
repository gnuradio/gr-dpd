/* -*- c++ -*- */
/*
 * Copyright 2020 gr-dpd author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DPD_RLS_POSTDISTORTER_H
#define INCLUDED_DPD_RLS_POSTDISTORTER_H

#include <dpd/api.h>
#include <gnuradio/sync_block.h>
#include <dpd/dpd_utils.h>

namespace gr {
  namespace dpd {

    /*!
     * \brief <+description of block+>
     * \ingroup dpd
     *
     */
    class DPD_API RLS_postdistorter : virtual public gr::sync_block
    {
     public:
      typedef std::shared_ptr<RLS_postdistorter> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of dpd::RLS_postdistorter.
       *
       * To avoid accidental use of raw pointers, dpd::RLS_postdistorter's
       * constructor is in a private implementation
       * class. dpd::RLS_postdistorter::make is the public interface for
       * creating new instances.
       */
      static sptr make(const std::vector<int> &dpd_params, int iter_limit);
    };

  } // namespace dpd
} // namespace gr

#endif /* INCLUDED_DPD_RLS_POSTDISTORTER_H */

