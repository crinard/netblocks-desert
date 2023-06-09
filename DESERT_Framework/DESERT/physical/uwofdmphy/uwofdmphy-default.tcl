#
# Copyright (c) 2017 Regents of the SIGNET lab, University of Padova.
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
# @file   uwofdmphy-default.tcl
# @author Sara Falleni
# @version 1.0.0

# Module/UW/OFDM/PHY set tx_power_consumption_ 3.3
# Module/UW/OFDM/PHY set rx_power_consumption_ 0.620

PacketHeaderManager set tab_(PacketHeader/OFDM) 1

#Power Consumption is in Watt as in the SEANet Modem 
Module/UW/OFDM/PHY set tx_power_consumption_ 10     
Module/UW/OFDM/PHY set rx_power_consumption_ 10
Module/UW/OFDM/PHY  set AcquisitionThreshold_dB_    15.0 
Module/UW/OFDM/PHY  set MaxTxSPL_dB_               185.8

Module/UW/OFDM/PHY  set BCH_N               15
Module/UW/OFDM/PHY  set BCH_K               11
Module/UW/OFDM/PHY  set BCH_T               1

Module/UW/OFDM/PHY  set FRAME_BIT           9152

