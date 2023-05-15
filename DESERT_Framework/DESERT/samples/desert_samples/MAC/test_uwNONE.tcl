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
# This script is used to test UW-CSMA-ALOHA protocol
# There are 4 nodes placed in line that can transmit each other 
# packets with a CBR (Constant Bit Rate) Application Module
#
# N.B.: This Example require WOSS installed for the Underwater Channel
#
# Author: Federico Favaro <favarofe@dei.unipd.it>
# Version: 1.0.0
#
# NOTE: tcl sample tested on Ubuntu 11.10, 64 bits OS
#
# Stack of the nodes
#   +-------------------------+
#   |  3. UW/nb_p             |
#   +-------------------------+
#   |  2. UW/CSMA_ALOHA       |
#   +-------------------------+
#   |  1. MPHY/BPSK/Underwater|
#   +-------------------------+
#           |         |    
#   +-------------------------+
#   |    UnderwaterChannel    |
#   +-------------------------+

######################################
# Flags to enable or disable options #
######################################
set opt(verbose) 			1
set opt(trace_files)		0
set opt(bash_parameters) 	0

#####################
# Library Loading   #
#####################
load libMiracle.so
load libMiracleBasicMovement.so
load libmphy.so
load libmmac.so
load libUwmStd.so
load libuwip.so
load libuwstaticrouting.so
load libuwmll.so
load libuwudp.so
load libuwnetblocks.so
load libnb_p.so
load libnb_p_recv.so
load libuwcbr.so
load libuwcsmaaloha.so

#############################
# NS-Miracle initialization #
#############################
# You always need the following two lines to use the NS-Miracle simulator
set ns [new Simulator]
$ns use-Miracle

##################
# Tcl variables  #
##################
set opt(nn)                 2.0 ;# Number of Nodes
set opt(starttime)          1	
set opt(stoptime)           1000000
set opt(txduration)         [expr $opt(stoptime) - $opt(starttime)] ;# Duration of the simulation
set opt(txpower)            180.0  ;#Power transmitted in dB re uPa
set opt(maxinterval_)       20.0
set opt(freq)               25000.0 ;#Frequency used in Hz
set opt(bw)                 5000.0	;#Bandwidth used in Hz
set opt(bitrate)            4800.0	;#bitrate in bps
set opt(ack_mode)           "setAckMode"
set opt(cbr_period) 120
set opt(rngstream)	1


if {$opt(bash_parameters)} {
	if {$argc != 3} {
		puts "The script requires three inputs:"
		puts "- the first for the seed"
		puts "- the second one is for the Poisson CBR period"
		puts "- the third one is the cbr packet size (byte);"
		puts "example: ns test_uw_csma_aloha_fully_connected.tcl 1 60 125"
		puts "If you want to leave the default values, please set to 0"
		puts "the value opt(bash_parameters) in the tcl script"
		puts "Please try again."
		return
	} else {
		set opt(rngstream)    [lindex $argv 0]
		set opt(cbr_period) [lindex $argv 1]
	}
}

global defaultRNG
for {set k 0} {$k < $opt(rngstream)} {incr k} {
	$defaultRNG next-substream
}

if {$opt(trace_files)} {
	set opt(tracefilename) "./test_uwcsmaaloha.tr"
	set opt(tracefile) [open $opt(tracefilename) w]
	set opt(cltracefilename) "./test_uwcsmaaloha.cltr"
	set opt(cltracefile) [open $opt(tracefilename) w]
} else {
	set opt(tracefilename) "/dev/null"
	set opt(tracefile) [open $opt(tracefilename) w]
	set opt(cltracefilename) "/dev/null"
	set opt(cltracefile) [open $opt(cltracefilename) w]
}

set channel [new Module/UnderwaterChannel]
set propagation [new MPropagation/Underwater]
set data_mask [new MSpectralMask/Rect]
$data_mask setFreq       $opt(freq)
$data_mask setBandwidth  $opt(bw)

#########################
# Module Configuration  #
#########################

Module/MPhy/BPSK  set TxPower_               $opt(txpower)

################################
# Procedure(s) to create nodes #
################################
proc createNodes {} {

    global channel propagation data_mask ns cbr position node udp portnum ipr ipif channel_estimator prnt
    global phy posdb opt rvposx rvposy rvposz mhrouting mll mac woss_utilities woss_creator db_manager
    global node_coordinates
    
    set node(0) [$ns create-M_Node $opt(tracefile) $opt(cltracefile)] 
    set node(1) [$ns create-M_Node $opt(tracefile) $opt(cltracefile)] 

    set prnt(0)  [new Module/UW/Nb_p_recv]
    set prnt(1)  [new Module/UW/Nb_p]
    set mac(0)  [new Module/UW/CSMA_ALOHA] 
    set mac(1)  [new Module/UW/CSMA_ALOHA] 
    set phy(0)  [new Module/MPhy/BPSK]  
    set phy(1)  [new Module/MPhy/BPSK]  
	

    $node(0) addModule 3 $prnt(0)   1  "PRNT"
    $node(0) addModule 2 $mac(0)   1  "MAC"
    $node(0) addModule 1 $phy(0)   1  "PHY"

    $node(1) addModule 3 $prnt(1)   1  "PRNT"
    $node(1) addModule 2 $mac(1)   1  "MAC"
    $node(1) addModule 1 $phy(1)   1  "PHY"

    $node(0) setConnection $prnt(0)   $mac(0)   0
    $node(0) setConnection $mac(0)   $phy(0)   1
    $node(0) addToChannel  $channel    $phy(0)   1

    $node(1) setConnection $prnt(1)   $mac(1)   0
    $node(1) setConnection $mac(1)   $phy(1)   1
    $node(1) addToChannel  $channel    $phy(1)   1

    #Set the IP address of the node
    
    set position(0) [new "Position/BM"]
    $node(0) addPosition $position(0)
    set posdb(0) [new "PlugIn/PositionDB"]
    $node(0) addPlugin $posdb(0) 20 "PDB"
    # $posdb(0) addpos [$ipif(0) addr] $position(0)

    set position(1) [new "Position/BM"]
    $node(1) addPosition $position(1)
    set posdb(1) [new "PlugIn/PositionDB"]
    $node(1) addPlugin $posdb(1) 20 "PDB"
    # $posdb(1) addpos [$ipif(1) addr] $position(1)
    
    #Setup positions
    $position(0) setX_ [expr 0*200]
    $position(0) setY_ [expr 0*200]
    $position(0) setZ_ -100

    $position(1) setX_ [expr 1*200]
    $position(1) setY_ [expr 1*200]
    $position(1) setZ_ -100
    
    #Interference model
    set interf_data(0) [new "MInterference/MIV"]
    $interf_data(0) set maxinterval_ $opt(maxinterval_)
    $interf_data(0) set debug_       0

    set interf_data(1) [new "MInterference/MIV"]
    $interf_data(1) set maxinterval_ $opt(maxinterval_)
    $interf_data(1) set debug_       0

	#Propagation model
    $phy(0) setPropagation $propagation

    $phy(1) setPropagation $propagation

    $phy(0) setSpectralMask $data_mask
    $phy(0) setInterference $interf_data(0)
    $mac(0) $opt(ack_mode)
    $mac(0) initialize

    $phy(1) setSpectralMask $data_mask
    $phy(1) setInterference $interf_data(1)
    $mac(1) $opt(ack_mode)
    $mac(1) initialize
    puts "$mac(0) setMacAddr 0"
    puts "$mac(1) setMacAddr 1"
}

#################
# Node Creation #
#################
# Create here all the nodes you want to network together
createNodes


# $prnt(1) settelem

#####################
# Start/Stop Timers #
#####################
# Set here the timers to start and/or stop modules (optional)
# e.g., 
for {set id1 0} {$id1 < $opt(nn)} {incr id1}  {
    $ns at $opt(starttime)    "$prnt($id1) start"
    $ns at $opt(stoptime)     "$prnt($id1) stop"
}

###################
# Final Procedure #
###################
# Define here the procedure to call at the end of the simulation
proc finish {} {
    global ns opt outfile
    global mac propagation cbr_sink mac_sink phy_data phy_data_sink channel db_manager propagation prnt
    global node_coordinates
    global ipr_sink ipr ipif udp cbr phy phy_data_sink prnt
    global node_stats tmp_node_stats sink_stats tmp_sink_stats
    if ($opt(verbose)) {
        puts "---------------------------------------------------------------------"
        puts "Simulation summary"
        puts "number of nodes  : $opt(nn)"
        # puts "app_period       : $opt(app_period) s"
        puts "number of nodes  : $opt(nn)"
        puts "simulation length: $opt(txduration) s"
        puts "tx power         : $opt(txpower) dB"
        puts "tx frequency     : $opt(freq) Hz"
        puts "tx bandwidth     : $opt(bw) Hz"
        puts "bitrate          : $opt(bitrate) bps"
        puts "---------------------------------------------------------------------"
    }
    set sum_throughput     0
    set sum_per                0
    set sum_sent_pkts      0.0
    set sum_rcv_pkts       0.0    

    for {set i 0} {$i < $opt(nn)} {incr i}  {
        puts "node $i [$prnt($i) getrecvpkts] recieved packets, [$prnt($i) getrecvbytes] bytes recieved, [$prnt($i) getsentpkts] sent packets, [$prnt($i) getsentbytes] bytes sent, [$prnt($i) getdroppkts] dropped packets"
    }
    
    $ns flush-trace
    close $opt(tracefile)
}


###################
# start simulation
###################
if ($opt(verbose)) {
    puts "\nStarting Simulation\n"
    puts "----------------------------------------------"
}


$ns at [expr $opt(stoptime) + 250.0]  "finish; $ns halt" 

$ns run
