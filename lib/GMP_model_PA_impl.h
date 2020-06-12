/* -*- c++ -*- */
/*
 * Copyright 2020 gr-dpd author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DPD_GMP_MODEL_PA_IMPL_H
#define INCLUDED_DPD_GMP_MODEL_PA_IMPL_H

#include <dpd/GMP_model_PA.h>
#include <math.h>
#include <algorithm>
#include <armadillo>
using std::vector;
using namespace arma;

namespace gr {
namespace dpd {

class GMP_model_PA_impl : public GMP_model_PA
{
private:
    int K_a;
    int L_a;
    int K_b;
    int M_b;
    int L_b;
    int M; // No. of Coefficients
    int Mode_vl;
public:
    GMP_model_PA_impl(int model_param1,
                      int model_param2,
                      int model_param3,
                      int model_param4,
                      int model_param5,
                      int mode);
    ~GMP_model_PA_impl();

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
    void gen_GMP_vector(const gr_complex* const in,
                        int item,
                        int K_a,
                        int L_a,
                        int K_b,
                        int M_b,
                        int L_b,
                        cx_fcolvec& GMP_vector);
};

} // namespace dpd
} // namespace gr

#endif /* INCLUDED_DPD_GMP_MODEL_PA_IMPL_H */
