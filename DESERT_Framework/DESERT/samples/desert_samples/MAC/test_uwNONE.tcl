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
set opt(nn)                 4.0 ;# Number of Nodes
set opt(pktsize)            125  ;# Pkt sike in byte
set opt(starttime)          1	
set opt(stoptime)           100000 
set opt(txduration)         [expr $opt(stoptime) - $opt(starttime)] ;# Duration of the simulation
set opt(txpower)            180.0  ;#Power transmitted in dB re uPa
set opt(maxinterval_)       20.0
set opt(freq)               25000.0 ;#Frequency used in Hz
set opt(bw)                 5000.0	;#Bandwidth used in Hz
set opt(bitrate)            4800.0	;#bitrate in bps
set opt(ack_mode)           "setNoAckMode"
set opt(cbr_period) 120
set opt(pktsize)	125
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
		set opt(pktsize)    [lindex $argv 2]
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
# Module/UW/CBR set packetSize_          $opt(pktsize)
# Module/UW/CBR set period_              $opt(cbr_period)
# Module/UW/CBR set PoissonTraffic_      1
# Module/UW/CBR set debug_               0

Module/MPhy/BPSK  set TxPower_               $opt(txpower)

################################
# Procedure(s) to create nodes #
################################
proc createNode { id } {

    global channel propagation data_mask ns cbr position node udp portnum ipr ipif channel_estimator prnt
    global phy posdb opt rvposx rvposy rvposz mhrouting mll mac woss_utilities woss_creator db_manager
    global node_coordinates
    
    set node($id) [$ns create-M_Node $opt(tracefile) $opt(cltracefile)] 

    set prnt($id)  [new Module/UW/Nb_p]
    set mac($id)  [new Module/UW/CSMA_ALOHA] 
    set phy($id)  [new Module/MPhy/BPSK]  
	

    $node($id) addModule 3 $prnt($id)   1  "PRNT"
    $node($id) addModule 2 $mac($id)   1  "MAC"
    $node($id) addModule 1 $phy($id)   1  "PHY"

    $node($id) setConnection $prnt($id)   $mac($id)   0
    $node($id) setConnection $mac($id)   $phy($id)   1
    $node($id) addToChannel  $channel    $phy($id)   1

    if {$id > 254} {
		puts "hostnum > 254!!! exiting"
		exit
    }
    #Set the IP address of the node
    set ip_value [expr $id + 1]
    
    set position($id) [new "Position/BM"]
    $node($id) addPosition $position($id)
    set posdb($id) [new "PlugIn/PositionDB"]
    $node($id) addPlugin $posdb($id) 20 "PDB"
    # $posdb($id) addpos [$ipif($id) addr] $position($id)
    
    #Setup positions
    $position($id) setX_ [expr $id*200]
    $position($id) setY_ [expr $id*200]
    $position($id) setZ_ -100
    
    #Interference model
    set interf_data($id) [new "MInterference/MIV"]
    $interf_data($id) set maxinterval_ $opt(maxinterval_)
    $interf_data($id) set debug_       0

	#Propagation model
    $phy($id) setPropagation $propagation
    
    $phy($id) setSpectralMask $data_mask
    $phy($id) setInterference $interf_data($id)
    $mac($id) $opt(ack_mode)
    $mac($id) initialize
}

#################
# Node Creation #
#################
# Create here all the nodes you want to network together
for {set id 0} {$id < $opt(nn)} {incr id}  {
    createNode $id
}


################################
# Inter-node module connection #
################################
proc connectNodes {id1 des1} {
    global ipif ipr portnum cbr cbr_sink ipif_sink ipr_sink opt 

    # $cbr($id1,$des1) set destAddr_ [$ipif($des1) addr]
    # $cbr($id1,$des1) set destPort_ $portnum($des1,$id1)

    # $cbr($des1,$id1) set destAddr_ [$ipif($id1) addr]
    # $cbr($des1,$id1) set destPort_ $portnum($id1,$des1) 

}

##################
# Setup flows    #
##################
for {set id1 0} {$id1 < $opt(nn)} {incr id1}  {
	for {set id2 0} {$id2 < $opt(nn)} {incr id2}  {
		connectNodes $id1 $id2
	}
}

#Print the routing tables of the nodes
#if {$opt(verbose)} {
#	for {set id1 0} {$id1 < $opt(nn)} {incr id1}  {
#		$ipr($id1) printroutes
#	}
#}



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
        puts "packet size      : $opt(pktsize) byte"
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
        set throughput           [$prnt($i) getthroughput]
        set sent_pkts        [$prnt($i) getsentpkts]
        set rcv_pkts           [$prnt($i) getrecvpkts]
        set sum_throughput [expr $sum_throughput + $throughput]
        set sum_sent_pkts [expr $sum_sent_pkts + $sent_pkts]
        set sum_rcv_pkts  [expr $sum_rcv_pkts + $rcv_pkts]
    }
    
    #if ($opt(verbose)) {
    #    puts "Mean Throughput          : [expr ($sum_cbr_throughput/(($opt(nn))*($opt(nn)-1)))]"
    #    puts "Sent Packets             : $sum_cbr_sent_pkts"
    #    puts "Received Packets         : $sum_cbr_rcv_pkts"
    #    puts "Packet Delivery Ratio    : [expr $sum_cbr_rcv_pkts / $sum_cbr_sent_pkts * 100]"
    #    # puts "IP Pkt Header Size       : $ipheadersize"
    #    # puts "UDP Header Size          : $udpheadersize"
    #    puts "CBR Header Size          : $cbrheadersize"
    #    puts "done!"
    #}
    
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
