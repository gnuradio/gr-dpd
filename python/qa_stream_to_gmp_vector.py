#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2020 gr-dpd author.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import dpd_swig as dpd

class qa_stream_to_gmp_vector(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_t(self):
        # set up fg
        self.tb.run()
        # check data


if __name__ == '__main__':
    gr_unittest.run(qa_stream_to_gmp_vector)
