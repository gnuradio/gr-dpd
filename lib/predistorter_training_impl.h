/* -*- c++ -*- */
/*
 * Copyright 2020 gr-dpd author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DPD_PREDISTORTER_TRAINING_IMPL_H
#define INCLUDED_DPD_PREDISTORTER_TRAINING_IMPL_H

#include <dpd/predistorter_training.h>
#include <armadillo>

namespace gr {
namespace dpd {

class predistorter_training_impl : public predistorter_training
{
private:
    // Nothing to declare in this block.
    int d_M;
    bool d_update_predistorter_training, update_predistorter_training;
    arma::cx_colvec d_predistorter_training_colvec, predistorter_training_colvec;

public:
    predistorter_training_impl(int M);
    ~predistorter_training_impl();

    void get_taps(pmt::pmt_t P);
    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace dpd
} // namespace gr

#endif /* INCLUDED_DPD_PREDISTORTER_TRAINING_IMPL_H */
