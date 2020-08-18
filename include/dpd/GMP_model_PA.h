/* -*- c++ -*- */
/*
 * Copyright 2020 Alekh Gupta
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DPD_GMP_MODEL_PA_H
#define INCLUDED_DPD_GMP_MODEL_PA_H

#include <gnuradio/sync_block.h>
#include <dpd/api.h>

namespace gr {
namespace dpd {

/*!
 * \brief Implementation of Generalised Memory Polynomial model
 *  based Power Amplifier. (Complex input, Complex output)
 * \ingroup dpd
 *
 * \details
 *  It produces an output based on the
 *  Generalised Memory Polynomial model formed using the current
 *  block parameters for each input.
 *
 *  Function to describe relation between Input and Output
 *  of GMP based model:
 *  \f[
 *    y[n] = \sum\limits_{k=0}^{K_a-1}(\sum\limits_{l=0}^{L_a-1} a_{kl} x(n-l)
 * |x(n-l)|^{k}) + \\
 *           \sum\limits_{k=1}^{K_b}(\sum\limits_{m=1}^{M_b}(\sum\limits_{l=0}^{L_b-1}
 * b_{kml} x(n-l) |x(n-l-m)|^{k})) \f] The representation on the right consists of two
 * components, namely, the signed-aligned-envelope component and signal-and-lagging
 * component. The number of coefficients to be estimated are: \f$M = K_a L_a + K_b M_b
 * L_b\f$ across across \f$K_a + K_b M_b\f$ channels.
 */
class DPD_API GMP_model_PA : virtual public gr::sync_block
{
public:
    typedef boost::shared_ptr<GMP_model_PA> sptr;

    /*!
     * \brief Make a GMP_model_PA block
     *
     * \param model_param1 Maximum Power Order(K_a) in the
     *  signal-and-aligned-envelope component of the GMP Model.
     * \param model_param2 Memory Depth(L_a) or
     *  Maximum number of terms of each power order in the
     *  signal-and-aligned-envelope component of the GMP Model.
     * \param model_param3 Maximum Power Order(K_b) in the
     *  signal-and-lagging-envelope component of the GMP Model.
     * \param model_param4 Lagging Cross-Terms Index(M_b) or
     *  Maximum number of terms due to Lagging Cross Terms for
     *  each memory depth in the signal-and-lagging-envelope
     *  component of the GMP Model.
     * \param model_param5 Memory-Depth(L_b) or
     *  Maximum number of terms of each power order in the
     *  signal-and-lagging-envelope component of the GMP Model.
     * \param mode Mode of Operation, i.e., Odd Order Terms Only
     *  or Even Order Terms Only or Both Terms.
     * \param coeff1 A shift-structured coefficients vector of
     *  complex data types of order (K_a x L_a) for the
     *  signal-and-aligned-envelope component.
     * \param coeff2 A shift-structured coefficients vector of
     *  complex data types of order (K_b x L_b x M_b) for the
     *  signal-and-lagging-envelope component.
     */
    static sptr make(int model_param1,
                     int model_param2,
                     int model_param3,
                     int model_param4,
                     int model_param5,
                     std::string mode,
                     const std::vector<gr_complex>& coeff1,
                     const std::vector<gr_complex>& coeff2);
};

} // namespace dpd
} // namespace gr

#endif /* INCLUDED_DPD_GMP_MODEL_PA_H */
