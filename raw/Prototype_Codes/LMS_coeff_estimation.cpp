// Implementation of LMS based coefficient estimator
/*
------------------------------------------------------------------------------------------------------------|
  Terminal Command to build this file: g++ LMS_coeff_estimation.cpp -O1 -llapack -lblas -larmadillo        
  Example Input Format:                                                                                     
------------------------------------------------------------------------------------------------------------|
  Estimate coefficients: y/n 
  y
  Enter PA_input(pre-distorted) - Real and Imag. parts:                                                     | 
  1.5 2.0
  Enter PA_output (Gain phase-calibrated) - Real and Imag. parts:                                           
  1.0 1.7                                                                                                   |
  K_a, L_a, K_b, M_b and L_b Parameters of PA model for postdistorter:                                       
  2 3 2 2 2                                                                                                  
  Enter the mode of LMS predistortion:                                                                      |
 
  Type Newton or EMA
                                                                                                            |
  Newton
 
  Error before Updation of weights: (0.300000,0.000000)                                                     |
                                                                                                            
  Taps: 
      (+1.110e+00,+0.000e+00)                                                                               |
      (-1.027e-02,+0.000e+00)
      (-1.027e-02,+0.000e+00)
      (+1.299e-02,+0.000e+00)                                                                               |
      (-1.027e-02,+0.000e+00)
      (-1.027e-02,+0.000e+00)
      (-1.027e-02,+0.000e+00)                                                                               |
      (-1.027e-02,+0.000e+00)
      (-1.027e-02,+0.000e+00)
      (-1.027e-02,+0.000e+00)                                                                               |
      (-1.027e-02,+0.000e+00)
      (-1.027e-02,+0.000e+00)
      (-1.027e-02,+0.000e+00)                                                                               |
      (-1.027e-02,+0.000e+00)

  Error after Updation of weights: (0.075000,0.000000)                                                      |
  Estimate coefficients: y/n                                                     
------------------------------------------------------------------------------------------------------------|
*/

#include<iostream>
#define ARMA_DONT_PRINT_ERRORS
#include<armadillo>
#include<vector>
#include<string>
#include<complex>

using namespace arma;

typedef std::complex <double> gr_complexd;
typedef std::complex <float> gr_complex;  

std::vector<int> dpd_params(5);
int K_a, K_b, L_a, L_b, M_b;
int M;
int iter_limit;
std::string str;
int iteration;

cx_mat w_iMinus1;
const int sreg_len = 50;
gr_complexd sreg[50];
const gr_complexd* ptr_sreg = sreg;
gr_complexd pa_input, error;

std::vector<gr_complexd> taps;

cx_mat ls_estimation(cx_mat A, gr_complexd y)
{
  cx_mat lambda(size(A*(A.t()))); 
  lambda.fill(0.001);
  // lambda(0, 0) = 0.001;
      
  cx_mat yy(1, 1);
  yy(0, 0) = y;

  // cx_mat den = A * yy;
  // cx_mat num = A * (A.t());
  // num = num + lambda*eye<cx_mat>( size(A*(A.t())) );
      
  cx_mat ls_result = solve((A*(A.t()) + lambda*eye<cx_mat>( size(A*(A.t())) )), A * yy);
  // cx_mat ls_result(size(w_iMinus1), fill::zeros);
  return ls_result;
}

// Function to form a shift structure GMP vector
void gen_GMPvector(const gr_complexd* const in,
                                           int item,
                                           int K_a,
                                           int L_a,
                                           int K_b,
                                           int M_b,
                                           int L_b,
                                           cx_colvec& GMP_vector)
  {
      /* Signal-and-Aligned Envelope */
      // stacking L_a elements in reverse order
      cx_colvec y_vec_arma1(L_a, fill::zeros);
      for (int ii = L_a - 1; ii >= 0; ii--)
          y_vec_arma1(ii) = in[item - ii];
      GMP_vector.rows(0, L_a - 1) = y_vec_arma1;

      // store abs() of y_vec_arma1
      cx_colvec abs_y_vec_arma1(size(y_vec_arma1), fill::zeros);
      abs_y_vec_arma1.set_real(abs(y_vec_arma1));

      cx_colvec yy_temp;
      yy_temp = y_vec_arma1 % abs_y_vec_arma1;
      if (K_a > 1)
          GMP_vector.rows(L_a, 2 * L_a - 1) = yy_temp;
      for (int kk = 2; kk < K_a; kk++) {
          // perform element-wise product using the overloaded % operator
          yy_temp = yy_temp % abs_y_vec_arma1;

          GMP_vector.rows(kk * L_a, (kk + 1) * L_a - 1) = yy_temp;
      }

      /* Signal-and-Delayed Envelope */
      // stacking L_b+M_b elements in reverse order
      cx_colvec y_vec_arma23(L_b + M_b, fill::zeros);
      for (int ii = L_b + M_b - 1; ii >= 0; ii--)
          y_vec_arma23(ii) = in[item - ii];

      // L_b signal elements
      cx_colvec y_vec_arma2 = y_vec_arma23.rows(0, L_b - 1);

      // store abs() of y_vec_arma23
      cx_colvec abs_y_vec_arma23(size(y_vec_arma23), fill::zeros);
      abs_y_vec_arma23.set_real(abs(y_vec_arma23));

      for (int mm = 1; mm < M_b + 1; mm++) {
          // stacking L_b delayed signal-envelope elements
          cx_colvec abs_y_vec_arma3 = abs_y_vec_arma23.rows(mm, L_b + mm - 1);

          cx_colvec yy_temp;
          yy_temp = y_vec_arma2 % abs_y_vec_arma3;
          GMP_vector.rows(K_a * L_a + (mm - 1) * K_b * L_b,
                          K_a * L_a + (mm - 1) * K_b * L_b + L_b - 1) = yy_temp;

          for (int kk = 2; kk < K_b + 1; kk++) {
                // perform element-wise product using the overloaded % operator
              yy_temp = yy_temp % abs_y_vec_arma3;

              GMP_vector.rows(K_a * L_a + (mm - 1) * K_b * L_b + (kk - 1) * L_b, 
                K_a * L_a + (mm - 1) * K_b * L_b + kk * L_b - 1) = yy_temp;
          }
      }
  }


// Initialise Parameters, weight vectors
void Init()
{	  
  iteration = 1;      
  for (int ii = 0; ii < sreg_len; ii++)   
    sreg[ii]=0.0;
  // taps or weight-vector
  w_iMinus1.set_size(M, 1);
  w_iMinus1 = zeros<cx_mat>(M, 1);
  w_iMinus1(0, 0) = gr_complex(1.0, 0.0);
      
}


int main()
{
  while(1)
  {
  	std::cout << "Estimate coefficients: y/n \n";
  	char c; 
    std::cin >> c;
  	if(c=='n')
  		break;
  	double x,z;
  	std::cout << "Enter PA_input(pre-distorted) - Real and Imag. parts: \n";
  	std::cin >> x >> z;
	  std::complex <double> PA_input = (x, z);
	  std::cout << "Enter PA_output (Gain phase-calibrated) - Real and Imag. parts: \n";
	  std::cin >> x >> z;
	  std::complex <double> Gain_cal_PA_output = (x, z);
	
	  std::cout << "K_a, L_a, K_b, M_b and L_b Parameters of PA model for postdistorter:\n";
	  std::cin >> dpd_params[0] >> dpd_params[1] >> dpd_params[2] >> dpd_params[3] >> dpd_params[4];
	  K_a = dpd_params[0];
	  L_a = dpd_params[1];
	  K_b = dpd_params[2];
  	M_b = dpd_params[3];
  	L_b = dpd_params[4];
  	M = dpd_params[0]*dpd_params[1] + dpd_params[2]*dpd_params[3]*dpd_params[4];
	
  	// std::cout << "Enter number of Iterations:\n";
  	// std::cin >> iter_limit;

    std::cout << "Enter the mode of LMS predistortion:\n \nType Newton or EMA\n \n";
    std::cin >> str;
  	Init();
                    
    pa_input = PA_input;
    sreg[49] = Gain_cal_PA_output;   

    // extracting the PA output and arranging into a shift-structured GMP vector
    cx_colvec GMP_vector(M);
    gen_GMPvector(ptr_sreg, 49, K_a, L_a, K_b, M_b, L_b, GMP_vector);
    cx_mat pa_output = GMP_vector;
    cx_mat pa_output_t = GMP_vector.t();

    for (int ii = 1; ii < sreg_len; ii++)
      sreg[ii - 1] = sreg[ii];

    cx_mat learning_rate = {0.75};
    cx_mat learning_rate_1_minus = {0.25};

    cout << std::fixed;
    
    // adaptation error
    error = pa_input - as_scalar(pa_output_t * w_iMinus1);
    cout << "\nError before Updation of weights: " << error << "\n";

    if(str == "Newton")
    {
      error = pa_input - as_scalar(pa_output_t * w_iMinus1);

      cx_mat ls_result = ls_estimation(pa_output, error);

      // update weight-vector
      w_iMinus1 = w_iMinus1 + (ls_result * learning_rate);
    }
    if(str == "EMA")
    {
      cx_mat ls_result = ls_estimation(pa_output, pa_input);

      // update weight-vector
      w_iMinus1  = (w_iMinus1 * learning_rate_1_minus) + (ls_result * learning_rate);
    }
    
    w_iMinus1.print("\nTaps: ");

    error = pa_input - as_scalar(pa_output_t * w_iMinus1);
    cout << "\nError after Updation of weights: " << error << "\n";
  }
}

