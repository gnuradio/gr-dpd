/* -*- c++ -*- */
/* 
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include<dpd/dpd_utils.h>
#include <armadillo>
#include <vector>
#include <algorithm>

using namespace arma;
using std::vector;

namespace gr{
	namespace dpd{

void gen_GMPvector(const gr_complex *const in, int item, 
  int K_a, int L_a, int K_b, int M_b, int L_b, cx_fcolvec &GMP_vector)
{
  /* Signal-and-Aligned Envelope */
  // stacking L_a elements in reverse order
  cx_fcolvec y_vec_arma1(L_a, fill::zeros); 
  for (int ii = L_a-1; ii >= 0; ii--) 
    y_vec_arma1(ii) = in[item-ii]; 
  GMP_vector.rows(0, L_a-1) = y_vec_arma1;  

  //store abs() of y_vec_arma1
  cx_fcolvec abs_y_vec_arma1( size(y_vec_arma1), fill::zeros );
  abs_y_vec_arma1.set_real( abs(y_vec_arma1) );

  cx_fcolvec yy_temp;
  yy_temp = y_vec_arma1%abs_y_vec_arma1;
  GMP_vector.rows(L_a, 2*L_a-1) = yy_temp;
  for (int kk = 2; kk<K_a; kk++) {
    // perform element-wise product using the overloaded % operator
    yy_temp = yy_temp%abs_y_vec_arma1;
  
    GMP_vector.rows(kk*L_a, (kk+1)*L_a-1) = yy_temp;
  }

  /* Signal-and-Delayed Envelope */
  // stacking L_b+M_b elements in reverse order
  cx_fcolvec y_vec_arma23(L_b+M_b, fill::zeros); 
  for (int ii = L_b+M_b-1; ii >= 0; ii--) 
    y_vec_arma23(ii) = in[item-ii]; 

  // L_b signal elements
  cx_fcolvec y_vec_arma2 = y_vec_arma23.rows(0, L_b-1);

  // store abs() of y_vec_arma23
  cx_fcolvec abs_y_vec_arma23( size(y_vec_arma23), fill::zeros );
  abs_y_vec_arma23.set_real( abs(y_vec_arma23) );

  for (int mm = 1; mm < M_b+1; mm++) {
    // stacking L_b delayed signal-envelope elements 
    cx_fcolvec abs_y_vec_arma3 = abs_y_vec_arma23.rows(mm, L_b+mm-1);

    cx_fcolvec yy_temp;
    yy_temp = y_vec_arma2%abs_y_vec_arma3;
    GMP_vector.rows(K_a*L_a+(mm-1)*K_b*L_b, K_a*L_a+(mm-1)*K_b*L_b+L_b-1) = yy_temp;
    for (int kk = 2; kk<K_b+1; kk++) {
      // perform element-wise product using the overloaded % operator
      yy_temp = yy_temp%abs_y_vec_arma3;

      GMP_vector.rows(K_a*L_a+(mm-1)*K_b*L_b+(kk-1)*L_b, K_a*L_a+(mm-1)*K_b*L_b+kk*L_b-1) = yy_temp;
    }
  }
}

void extract_g_vecs(cx_mat &g, cx_mat &g_vec_iMinus1, cx_mat &g_vec_i, int K_a, int L_a, int K_b, int M_b, int L_b, int M, int M_bar) 
{
  cx_mat g_vec_i1( K_a*L_a, 1, fill::zeros);
  cx_mat g_tmp1 = g( span(0, K_a*(L_a+1)-1), 0);
  g_tmp1.reshape(L_a+1, K_a);
  g_tmp1.shed_row(L_a);
  g_vec_i1 = vectorise(g_tmp1, 0);
  g_tmp1.insert_rows(0, 1);
  cx_mat g_vec_iMinus1_a = vectorise(g_tmp1, 0);

  cx_mat g_vec_i2_tmp(K_b*L_b, M_b, fill::zeros);
  cx_mat g_vec_iMinus1_b_tmp(K_b*(L_b+1), M_b, fill::zeros);
  for (int mm = 1; mm < M_b+1; mm++) 
  {
    int index1 = K_a*(L_a+1) + (mm-1)*K_b*(L_b+1);
    int index2 = K_a*(L_a+1) + (mm-1)*K_b*(L_b+1)+K_b*(L_b+1);

    cx_mat g_tmp3 = g( span(index1, index2-1), 0);
    g_tmp3.reshape(L_b+1, K_b);
    g_tmp3.shed_row(L_b);
    g_vec_i2_tmp.col(mm-1) = vectorise(g_tmp3, 0);
    g_tmp3.insert_rows(0, 1);
    g_vec_iMinus1_b_tmp.col(mm-1) = vectorise(g_tmp3, 0);
  }
  cx_mat g_vec_i2 = vectorise(g_vec_i2_tmp, 0);
  cx_mat g_vec_iMinus1_b = vectorise(g_vec_iMinus1_b_tmp, 0);

  g_vec_i.rows(0, K_a*L_a-1) = g_vec_i1;
  g_vec_i.rows(K_a*L_a, M-1) = g_vec_i2;

  g_vec_iMinus1.rows(0, K_a*(L_a+1)-1) = g_vec_iMinus1_a;
  g_vec_iMinus1.rows(K_a*(L_a+1), M_bar-1) = g_vec_iMinus1_b;
}

void extract_postdistorted_y(cx_fmat &y_in, cx_fmat &y, int K_a, int L_a, int K_b, int M_b, int L_b, int M) 
{
  cx_fmat y_vec_1( 1, K_a*L_a, fill::zeros);
  cx_fmat y_tmp1 = y_in( 0, span(0, K_a*(L_a+1)-1) );

  y_tmp1.reshape(L_a+1, K_a);
  y_tmp1.shed_row(L_a);
  y_vec_1 = vectorise(y_tmp1, 0);

  cx_fmat y_vec_2_tmp(K_b*L_b, M_b, fill::zeros);
  for (int mm = 1; mm < M_b+1; mm++) {
    int index1 = K_a*(L_a+1) + (mm-1)*K_b*(L_b+1);
    int index2 = K_a*(L_a+1) + (mm-1)*K_b*(L_b+1)+K_b*(L_b+1);

    cx_fmat y_tmp3 = y_in( 0, span(index1, index2-1) );
    y_tmp3.reshape(L_b+1, K_b);
    y_tmp3.shed_row(L_b);
    y_vec_2_tmp.col(mm-1) = vectorise(y_tmp3, 0);
  }
  cx_fmat y_vec_2 = vectorise(y_vec_2_tmp, 0);
  
  y.cols(0, K_a*L_a-1) = strans(y_vec_1);
  y.cols(K_a*L_a, M-1) = strans(y_vec_2);
}

bool almost_equal(double a, double b, double tol)
{
  // calculate the difference
  double diff_ab = fabs(a - b);
  
  if (diff_ab <= tol*std::numeric_limits<double>::epsilon())
    return true;

  return false;
}

bool almost_equals_zero(double a, int num_digits)
{
  // identify the first few significant digits
  int na = floor(fabs(a)*pow(10.0, num_digits));
  
  if (na == 0)
    return true;

  return false;
}

void givens_rotate(const cx_mat & in, cx_mat & out) 
{
  const static gr_complexd minus_1i(0, -1);

  cx_mat theta(2, 2, fill::zeros);
  gr_complexd alpha; 
  double scale, norm;
  cx_mat g(2, 2, fill::ones);
    
  // algorithm similar to LAPACK's crotg()
  // except for the application of scale adjustment matrix, g
  double a = abs(in(0, 0));
  double b = abs(in(0, 1));
  if (almost_equal(b, 0.0, 100.0)) {
    theta(0,0) = 1.0;
    theta(1,1) = 1.0;

    // scale adjustment matrix to make the first element of post-array real
    g(0, 0) = std::exp(minus_1i*std::arg(in(0, 0)));
    g(0, 1) = gr_complexd(0.0, 0.0);
    g(1, 0) = gr_complexd(0.0, 0.0);
    g(1, 1) = g(0, 0);    
  }
  else if (almost_equal(a, 0.0, 100.0)) { 
    theta(0,1) = 1.0;
    theta(1,0) = -1.0;

    // scale adjustment matrix to make the first element of post-array real
    g(0, 1) = std::exp(minus_1i*std::arg(in(0, 1)));
    g(1, 0) = g(0,1);    
  }
  else {
    scale = a+b;
    norm = scale*sqrt(pow(abs(in(0, 0)/scale), 2)+ pow(abs(in(0, 1)/scale), 2));
    alpha = in(0, 0)/a;
    theta(0,0) = a/norm;
    theta(1,1) = theta(0,0);
    theta(1,0) = alpha*conj(in(0, 1))/norm;
    theta(0,1) = -conj(theta(1,0));

    // scale adjustment matrix to make the first element of post-array real
    g(0, 0) = std::exp(minus_1i*std::arg(in(0, 0)));
    g(0, 1) = g(0,0);
    g(1, 0) = g(0,0);
    g(1, 1) = g(0,0);  
  }
  theta = g%theta;
  
  // form post-array
  out = in*theta;
}

void hgivens_rotate(const cx_mat & in, cx_mat & out) 
{
  const static gr_complexd minus_1i(0, -1);

  int flip; 
  gr_complexd a, b, c, d1, d2, x, y, z, conj_a, conj_b, p, q;
  double scale, angle_a;

  x = in(0, 0);
  y = in(0, 1);
  if (almost_equal(abs(x), 0.0, 100.0)) {
    angle_a = std::arg(in(0, 1));
    out = std::exp(minus_1i*angle_a)*in;
  }
  else if (almost_equal(abs(y), 0.0, 100.0)) {
    angle_a = std::arg(in(0, 0));
    out = std::exp(minus_1i*angle_a)*in;
  }
  else {
    b = in(0, 1);
    a = in(0, 0);        
    conj_a = conj(a);
    conj_b = conj(b);        
    if ( abs(in(0, 0)) > abs(in(0, 1)) ) {
      flip = 0;     
      scale = abs(a)/sqrt((abs(a) - abs(b)) * (abs(a) + abs(b)));
      d1 = (conj_a-conj_b)/conj_a;
      p = (a+b)/a;
      q = (conj_a+conj_b)/conj_a;   
    }
    else {
      flip = 1;        
      scale = abs(b)/sqrt((abs(b) - abs(a)) * (abs(a) + abs(b)));
      d1 = (b-a)/b;
      p = (conj_a+conj_b)/conj_b;
      q = (a+b)/b;                
    }

    // Algorithm adapted from H-procedure described in 2.6.4 and 2.A of 
    // Fast Reliable Algorithms for Matrices with Structure - Edited by Sayed and Kailath
    for (int ii = 0; ii < in.n_rows; ii++) {
      if ( almost_equal(abs(in(ii, 0)), 0.0, 100.0) && almost_equal(abs(in(ii, 1)), 0.0, 100.0) ) {
        out(ii, 0) = in(ii, 0);
        out(ii, 1) = in(ii, 1); 
      }
      else {
        x = in(ii, 0);
        y = in(ii, 1);    
        if ( !almost_equal(abs(in(ii, 0)), 0.0, 100.0) ) {
          d2 = (x-y)/x;
          z = d1+d2-d1*d2;

          out(ii, 0) = scale * x * z;
          out(ii, 1) = out(ii, 0) - scale*( p*x - q*y );
        }
        else {
          out(ii, 0) = -scale*y*(q-gr_complexd(1.0, 0.0));
          out(ii, 1) = scale*y;                
        }
      }
    }
    
    if (flip == 0)
        out = std::exp(minus_1i*std::arg(out(0,0)))*out;
    else
        out = std::exp(minus_1i*std::arg(out(0,1)))*out;    
  }
}

void apply_rotations(const cx_mat & A, cx_mat & B) 
{
  cx_mat A_bar = A;
       
  for (int i = 1; i < A_bar.n_cols; i++) {				    
    cx_mat z(A_bar.n_rows, 2, fill::zeros);
    z.col(0) = A_bar.col(0);
    z.col(1) = A_bar.col(i);

    cx_mat rotated_z(z.n_rows, 2, fill::zeros);
    switch( i%2 ) {
      case 1:
        // apply Givens rotation to z
        givens_rotate(z, rotated_z);		
        break;

      case 0: 
        // apply Hyperbolic-Givens rotation to z
        hgivens_rotate(z, rotated_z);
        break;
    }
	     
    B.col(0) = rotated_z.col(0);            
    if ( i>1 && i<A_bar.n_cols-1 ) {
      B.cols(1, i-1) = A_bar.cols(1, i-1);
      B.col(i) = rotated_z.col(1);
      B.cols(i+1, B.n_cols-1) = A_bar.cols(i+1, A_bar.n_cols-1);
    }
    else if ( i==1 ) {
      B.col(1) = rotated_z.col(1);
      B.cols(2, B.n_cols-1) = A_bar.cols(2, A_bar.n_cols-1);
    }
    else if ( i==A_bar.n_cols-1 ) {
      B.cols(1, B.n_cols-2) = A_bar.cols(1, A_bar.n_cols-2);
      B.col(B.n_cols-1) = rotated_z.col(1);
    }
    A_bar = B;
  }
}

}
}



