/* -*- c++ -*- */
/*
 * Copyright 2020 Alekh Gupta
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
    const int K_a;
    const int L_a;
    const int K_b;
    const int M_b;
    const int L_b;
    const int d_M;
    bool d_update_predistorter_training, update_predistorter_training;
    arma::cx_colvec d_predistorter_training_colvec, predistorter_training_colvec;

public:
    predistorter_training_impl(const std::vector<int>& dpd_params);
    ~predistorter_training_impl();

    void get_taps(pmt::pmt_t P);
    void gen_GMPvector(const gr_complex* const in,
                        int item,
                        int K_a,
                        int L_a,
                        int K_b,
                        int M_b,
                        int L_b,
                        arma::cx_fcolvec& GMP_vector);
    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace dpd
} // namespace gr

#endif /* INCLUDED_DPD_PREDISTORTER_TRAINING_IMPL_H */
