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

# export LD_DEBUG all
AM_CXXFLAGS = -pedantic -Wall \
    -Wno-missing-braces -Wextra -Wformat=2 \
    -Wswitch-default -Wswitch-enum -Wcast-align -Wpointer-arith \
    -Winline \
    -Wundef -Wcast-qual -Wshadow -Wunreachable-code \
    -Wfloat-equal -Wstrict-aliasing=2 -Wredundant-decls \
    -Wno-write-strings \
    -ggdb3 \
    -O0 \
    -lm \
    -fPIC

lib_LTLIBRARIES = libnb_p_recv.la
check_PROGRAMS = 

SUBDIRS = 

TESTS = 

libnb_p_recv_la_SOURCES = initlib.cpp\
    nb_p_recv-default.tcl\
    nb_p_recv.cpp

libnb_p_recv_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@ @DESERT_CPPFLAGS@ -I /home/crinard/Desktop/DESERT_Underwater/DESERT_Framework/DESERT/network/nb_p_recv/net-blocks/scratch/ -I /home/crinard/Desktop/DESERT_Underwater/DESERT_Framework/DESERT/network/nb_p_recv/net-blocks/runtime/nbp_data_queue.h -I /home/crinard/Desktop/DESERT_Underwater/DESERT_Framework/DESERT/network/nb_p_recv/net-blocks/runtime -I /home/crinard/Desktop/DESERT_Underwater/DESERT_Framework/DESERT/network/nb_p_recv/net-blocks/scratch 
libnb_p_recv_la_LDFLAGS =  @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@ @DESERT_LDFLAGS@

libnb_p_recv_la_LIBADD = @NS_LIBADD@ @NSMIRACLE_LIBADD@ @DESERT_LIBADD@

nodist_libnb_p_recv_la_SOURCES = InitTcl.cc

BUILT_SOURCES = InitTcl.cc

CLEANFILES = InitTcl.cc

TCL_FILES = nb_p_recv-default.tcl

InitTcl.cc: Makefile $(TCL_FILES)
		cat $(VPATH)/$(TCL_FILES) | @TCL2CPP@ Nb_p_recvInitTclCode > InitTcl.cc

EXTRA_DIST = $(TCL_FILES)
