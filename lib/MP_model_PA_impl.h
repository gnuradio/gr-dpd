/* -*- c++ -*- */
/*
 * Copyright 2020 gr-dpd author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DPD_MP_MODEL_PA_IMPL_H
#define INCLUDED_DPD_MP_MODEL_PA_IMPL_H

#include <dpd/MP_model_PA.h>
#include <math.h>
#include <algorithm>
#include <armadillo>
using std::vector;
using namespace arma;

namespace gr {
namespace dpd {

class MP_model_PA_impl : public MP_model_PA
{
private:
    int K_a; // No. of channels or order of PA model
    int L_a; // No. of taps or memory-depth
public:
    MP_model_PA_impl(int Order, int Mem_Depth);
    ~MP_model_PA_impl();

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
    void gen_MP_vector(
        const gr_complex* in, int item, int K_a, int L_a, cx_fcolvec& MP_vector);
};

} // namespace dpd
} // namespace gr

#endif /* INCLUDED_DPD_MP_MODEL_PA_IMPL_H */
