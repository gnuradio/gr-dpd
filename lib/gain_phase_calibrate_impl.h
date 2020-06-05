/* -*- c++ -*- */
/*
 * Copyright 2020 gr-dpd author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DPD_GAIN_PHASE_CALIBRATE_IMPL_H
#define INCLUDED_DPD_GAIN_PHASE_CALIBRATE_IMPL_H

#include <dpd/gain_phase_calibrate.h>

namespace gr {
  namespace dpd {

    class gain_phase_calibrate_impl : public gain_phase_calibrate
    {
     private:
      gr_complex cfactor, previous_cfactor, current_cfactor;    
      int ninput_items, item;    
      bool d_reference_acquired, reference_acquired;
      gr_complex d_sample, sample;


     public:
      gain_phase_calibrate_impl();
      ~gain_phase_calibrate_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);
      void set_reference(pmt::pmt_t P);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);

    };

  } // namespace dpd
} // namespace gr

#endif /* INCLUDED_DPD_GAIN_PHASE_CALIBRATE_IMPL_H */

