#
# Copyright (c) 2015 Regents of the SIGNET lab, University of Padova.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of the University of Padova (SIGNET lab) nor the 
#    names of its contributors may be used to endorse or promote products 
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
# TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Author: Filippo Campagnaro
# Author: Roberto Francescon
# version: 1.0.0

Module/UW/TDMA set debug_ 		0
Module/UW/TDMA set HDR_size_ 		0
Module/UW/TDMA set ACK_size_  		10
Module/UW/TDMA set max_tx_tries_	5
Module/UW/TDMA set wait_constant_	0.1
Module/UW/TDMA set uwTDMA_debug_	0
Module/UW/TDMA set max_payload_		125
Module/UW/TDMA set ACK_timeout_		5.0
Module/UW/TDMA set alpha_		0.8
Module/UW/TDMA set buffer_pkts_		-1
Module/UW/TDMA set backoff_tuner_   	1
Module/UW/TDMA set max_backoff_counter_ 4
Module/UW/TDMA set MAC_addr_ 		0

Module/UW/TDMA set sea_trial_ 		0
Module/UW/TDMA set frame_time           0
Module/UW/TDMA set guard_time           0
Module/UW/TDMA set tot_slots            0
Module/UW/TDMA set fair_mode            0

Module/UW/TDMA instproc init {args} {
    $self next $args
    $self settag "UW/TDMA"
}
