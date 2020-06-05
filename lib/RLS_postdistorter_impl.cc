/* -*- c++ -*- */
/*
 * Copyright 2020 gr-dpd author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <gnuradio/io_signature.h>
#include "RLS_postdistorter_impl.h"
#include <armadillo>

#define COPY_MEM false  // Do not copy matrices into separate memory
#define FIX_SIZE true // Keep dimensions of matrices constant

using namespace std;
using std::string;
using namespace arma;

namespace gr {
  namespace dpd {

    RLS_postdistorter::sptr
    RLS_postdistorter::make(const std::vector<int> &dpd_params, int iter_limit)
    {
      return gnuradio::get_initial_sptr
        (new RLS_postdistorter_impl(dpd_params, iter_limit));
    }


    /*
     * The private constructor
     */
    RLS_postdistorter_impl::RLS_postdistorter_impl(const std::vector<int> &dpd_params, int iter_limit)
      : gr::sync_block("RLS_postdistorter",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(0, 0, 0)),
        d_dpd_params(dpd_params),
        K_a(d_dpd_params[0]), 
        L_a(d_dpd_params[1]),
        K_b(d_dpd_params[2]),
              M_b(d_dpd_params[3]),
              L_b(d_dpd_params[4]),
              M(dpd_params[0]*dpd_params[1] + dpd_params[2]*dpd_params[3]*dpd_params[4]),
              M_bar(dpd_params[0] + dpd_params[2]*dpd_params[3]),
              d_iter_limit(iter_limit)
    {
      d_ack_predistorter_updated = false;

      // setup output message port  
      message_port_register_out(pmt::mp("taps"));

      // setup input message port
      message_port_register_in(pmt::mp("PA_input"));
      set_msg_handler(pmt::mp("PA_input"), boost::bind(&RLS_postdistorter_impl::get_PA_input, this, _1));
      
      iteration = 1;      

      for (int ii = 0; ii < sreg_len; ii++)   
        sreg[ii]=0.0;

      for (int ii=0; ii < 10; ii++)
      {
        sr1[ii]=gr_complexd(0.0,0.0);
        sr2[ii]=gr_complexd(0.0,0.0);
      }

      // constants
      int k = 16;
      lambda = 1-pow(2, 1-k);
      one_over_sqrt_lambda = 1.0/sqrt(lambda);
      eta = pow(2, k);

      g_vec_iMinus1.set_size(M+M_bar, 1);
      g_vec_i.set_size(M, 1);
      L_bar_iMinus1.set_size(M+M_bar, M_bar*2);
      w_i.set_size(M, 1);
      w_iMinus1.set_size(M, 1);

      // inverse of square-root of gamma
      inv_sqrt_gamma_iMinus1 = 1;

      // g vector
      g_vec_iMinus1 = zeros<cx_mat>(M+M_bar, 1);

      // L-bar matrix
      cx_mat temp_cx_mat1(L_a+1, 2, fill::zeros);
      temp_cx_mat1(0, 0) = gr_complexd(sqrt(eta*lambda), 0);
      temp_cx_mat1(L_a, 1) = sqrt(eta*lambda)*pow(lambda, 0.5*L_a);
      cx_mat eye_K_a(K_a, K_a, fill::eye);
      cx_mat L_bar_iMinus1_a = kron(eye_K_a, temp_cx_mat1);
      
      cx_mat temp_cx_mat2(L_b+1, 2, fill::zeros);
      temp_cx_mat2(0, 0) = gr_complexd(sqrt(eta*lambda), 0);
      temp_cx_mat2(L_b, 1) = sqrt(eta*lambda)*pow(lambda, 0.5*L_b);
      cx_mat eye_K_bM_b(K_b*M_b, K_b*M_b, fill::eye);
      cx_mat L_bar_iMinus1_b = kron(eye_K_bM_b, temp_cx_mat2);
      
      L_bar_iMinus1 = zeros<cx_mat>(K_a*(L_a+1)+K_b*M_b*(L_b+1), (K_a+K_b*M_b)*2);
      L_bar_iMinus1( span(0, K_a*(L_a+1)-1), span(0, K_a*2-1) ) = L_bar_iMinus1_a;
      L_bar_iMinus1( span(K_a*(L_a+1), K_a*(L_a+1)+K_b*M_b*(L_b+1)-1), span(K_a*2, (K_a+K_b*M_b)*2-1) ) = L_bar_iMinus1_b;
      
      //weight-vector
      w_iMinus1 = zeros<cx_mat>(M, 1);
      w_iMinus1(0,0) = gr_complexd(1.0, 0.0);

      // A and B matrices
      A_mat.set_size(M+M_bar+1, M_bar*2+1);
      B_mat.set_size(M+M_bar+1, M_bar*2+1); 
      
      yy_cx_fcolvec.set_size(M+M_bar);       
      yy_cx_frowvec.set_size(M+M_bar);       
      y.set_size(1, M);    
      g.set_size(M+M_bar-1, 1);
    }

    /*
     * Our virtual destructor.
     */
    RLS_postdistorter_impl::~RLS_postdistorter_impl()
    {
    }

    void 
    RLS_postdistorter_impl::get_PA_input(pmt::pmt_t P) 
    { 
      d_pa_input = pmt::to_complex(P);
      d_ack_predistorter_updated = true;
    }

    int
    RLS_postdistorter_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      

      // Do <+signal processing+>
      // copy private variables accessed by the asynchronous message handler block
      pa_input = d_pa_input;
      ack_predistorter_updated = d_ack_predistorter_updated;
      
      for (int item = 0; item < noutput_items; item++) 
      {
        // get number of samples consumed since the beginning of time by this block
        // from port 0
        const uint64_t nread = this->nitems_read(0);        


        if ( ack_predistorter_updated ) 
        {
          std::cout << "Iteration Number: " << iteration << std::endl;                

          // extracting the PA output and arranging into a shift-structured GMP vector
          // sreg[49] = pa_output_smooth; 
          sreg[49] = in[item];                   
          gen_GMPvector(ptr_sreg, 49, K_a, L_a+1, K_b, M_b, L_b+1, yy_cx_fcolvec);
          yy_cx_frowvec = yy_cx_fcolvec.st();
          for (int ii = 1; ii < sreg_len; ii++)
            sreg[ii-1] = sreg[ii];          
     
          // A-matrix
          A_mat.submat(0, 0, 0, 0) = inv_sqrt_gamma_iMinus1;
          A_mat.submat(0, 1, 0, M_bar*2) = yy_cx_frowvec*L_bar_iMinus1;
          A_mat.submat(1, 0, M+M_bar, 0) = g_vec_iMinus1;
          A_mat.submat(1, 1, M+M_bar, M_bar*2) = L_bar_iMinus1;

          // obtain B-matrix by performing Givens and Hyperbolic Givens rotations
          apply_rotations(A_mat, B_mat);       
    
          // time-update for 1/sqrt(gamma)
          inv_sqrt_gamma_iMinus1 = real(B_mat(0, 0)); // imag(B_mat(0, 0)) will be ~= 0.0          
          std::cout << "inv_sqrt_gamma_iMinus1: " << inv_sqrt_gamma_iMinus1 << std::endl;
            
          // time-update for g-vector
          g = B_mat(span(1, M+M_bar), 0);    
          extract_g_vecs(g, g_vec_iMinus1, g_vec_i, K_a, L_a, K_b, M_b, L_b, M, M+M_bar);              
          
          // adjust post-distorted PA output dimensions
          extract_postdistorted_y(yy_cx_frowvec, y, K_a, L_a, K_b, M_b, L_b, M);
          
          // adaptation error
          error = pa_input - as_scalar(y*w_iMinus1);          

          // update weight-vector
          w_iMinus1 = w_iMinus1+(error/inv_sqrt_gamma_iMinus1)*g_vec_i;     
  
          // prepare L_bar for next iteration
          L_bar_iMinus1 = gr_complexd(one_over_sqrt_lambda, 0.0) * B_mat( span(1, M+M_bar), span(1, 2*M_bar) );          

          // send weight-vector to predistorter block in a message
          taps = conv_to< vector<gr_complexd> >::from(w_iMinus1);
          pmt::pmt_t P_c32vector_taps = pmt::init_c64vector(M, taps);
          message_port_pub(pmt::mp("taps"), P_c32vector_taps);

          iteration++;
          if ( iteration == d_iter_limit ) 
          {
            return(-1);
          }
          
          ack_predistorter_updated = false;
        } 
      }
      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace dpd */
} /* namespace gr */

