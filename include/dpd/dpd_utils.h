#ifndef INCLUDED_DPD_UTILS_H
#define INCLUDED_DPD_UTILS_H

#include <dpd/api.h>
#include<gnuradio/math.h>
#include <armadillo>
#include <vector>
#include <algorithm>

namespace gr {
  namespace dpd {

		void gen_GMPvector(const gr_complex *const in, int item, int K_a, int L_a, int K_b, int M_b, int L_b, arma::cx_fcolvec &GMP_vector);
		void extract_g_vecs(arma::cx_mat &g, arma::cx_mat &g_vec_iMinus1, arma::cx_mat &g_vec_i, int K_a, int L_a, int K_b, int M_b, int L_b, int M, int M_bar);
		void extract_postdistorted_y(arma::cx_fmat &y_in, arma::cx_fmat &y, int K_a, int L_a, int K_b, int M_b, int L_b, int M);  
		bool almost_equal(double a, double b, double tol);
		bool almost_equals_zero(double a, int num_digits);
		void givens_rotate(const arma::cx_mat & in, arma::cx_mat & out);
		void hgivens_rotate(const arma::cx_mat & in, arma::cx_mat & out);
		void apply_rotations(const arma::cx_mat & A, arma::cx_mat & B); 
  }/* namespace dpd */
}/* namespace gr */
#endif /* INCLUDED_DPD_UTILS_H */
