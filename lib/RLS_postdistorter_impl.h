/* -*- c++ -*- */
/*
 * Copyright 2020 gr-dpd author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DPD_RLS_POSTDISTORTER_IMPL_H
#define INCLUDED_DPD_RLS_POSTDISTORTER_IMPL_H

#include <dpd/RLS_postdistorter.h>
#include <armadillo>
using namespace arma;

namespace gr {
namespace dpd {

class RLS_postdistorter_impl : public RLS_postdistorter
{
private:
    // Nothing to declare in this block.
    std::vector<int> d_dpd_params;
    gr_complexd d_pa_input, pa_input, error;
    gr_complexd pa_input_smooth, pa_output_smooth;
    gr_complexd sr1[10], sr2[10];
    bool d_ack_predistorter_updated, ack_predistorter_updated;
    const int K_a, L_a, K_b, M_b, L_b, M, M_bar;
    const int sreg_len = 50;
    gr_complex sreg[50];
    const gr_complex* ptr_sreg = sreg;
    std::vector<gr_complexd> taps;

    int iteration, d_iter_limit;

    // Fast-RLS parameter declaration
    double lambda, one_over_sqrt_lambda, eta, inv_sqrt_gamma_iMinus1;
    cx_mat g_vec_iMinus1, g_vec_i, L_bar_iMinus1, w_i, w_iMinus1, A_mat, B_mat, g;
    cx_fcolvec yy_cx_fcolvec;
    cx_frowvec yy_cx_frowvec;
    cx_fmat y;

public:
    RLS_postdistorter_impl(const std::vector<int>& dpd_params, int iter_limit);
    ~RLS_postdistorter_impl();

    void get_PA_input(pmt::pmt_t P);
    bool almost_equal(double a, double b, double tol);
    void givens_rotate(const cx_mat& in, cx_mat& out);
    void hgivens_rotate(const cx_mat& in, cx_mat& out);
    void extract_g_vecs(cx_mat& g,
                        cx_mat& g_vec_iMinus1,
                        cx_mat& g_vec_i,
                        int K_a,
                        int L_a,
                        int K_b,
                        int M_b,
                        int L_b,
                        int M,
                        int M_bar);
    void extract_postdistorted_y(
        cx_fmat& y_in, cx_fmat& y, int K_a, int L_a, int K_b, int M_b, int L_b, int M);
    void apply_rotations(const cx_mat& A, cx_mat& B);
    void gen_GMPvector(const gr_complex* const in,
                       int item,
                       int K_a,
                       int L_a,
                       int K_b,
                       int M_b,
                       int L_b,
                       cx_fcolvec& GMP_vector);
    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace dpd
} // namespace gr

#endif /* INCLUDED_DPD_RLS_POSTDISTORTER_IMPL_H */
