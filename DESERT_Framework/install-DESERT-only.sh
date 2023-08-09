#!/bin/sh

# Copyright (c) 2014 Regents of the SIGNET lab, University of Padova.
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

# @name_file:   install.sh
# @author:      Ivano Calabrese
# @last_update: 2014.01.14
# --
# @brief_description:

# #############################################################################
# TODO:
# (v) change "installationLOG" in "install.log"                                 [2014.01.15]
# (-) add mutual exclusiveness between "command-line" and "wizard procedure"    [2014.00.00]

# INCLUDE
. ./commonVariables.sh
. ./commonFunctions.sh

#------------------------------------------------------------------------------
print_desert_logo
#------------------------------------------------------------------------------

log_L1 "_________________________" ${INSTALL_LOG}
log_L1 "- install.sh is STARTED -" ${INSTALL_LOG}

printf '%s\n' "$0 " > ${INSTALL_CONF}

check_sh
#PRIVATE_VARIABLEs
SLEEP05=0.5

DEST_FOLDER="/home/crinard/Desktop/DESERT_Underwater/DESERT_buildCopy_LOCAL"
BUILD_HOST="${DEST_FOLDER}/.buildHost"
BUILD_TARGET="${DEST_FOLDER}/.buildTarget"
TARGET="LOCAL"
INST_MODE="development"
BUILD_HOST="/home/crinard/Desktop/DESERT_Underwater/DESERT_buildCopy_LOCAL/.buildHost"
BUILD_TARGET="/home/crinard/Desktop/DESERT_Underwater/DESERT_buildCopy_LOCAL/.buildTarget"
CUSTOM_PAR=""
ADDONS=""


#***
# internal settings
#
#*

info_L0 "make_dir"
make_dir_modified
if [ $? -ne 0 ]; then
    err_L1 "EXIT FROM install.sh"
    exit
fi

current_dir=$PWD
echo "current_dir"
cd $current_dir/DESERT/network/nb_p/net-blocks/
make simple_desert DEBUG=1
echo "current_dir1"
cd $current_dir

info_L0 "call_installer"
call_installer_modified
if [ $? -ne 0 ]; then
    err_L1 "EXIT FROM install.sh"
    exit
fi
if [ ${_ADDONS} -ne 1 ]; then
    delete_recursive_soft_link
fi
info_L0 "after_building"
after_building
if [ $? -ne 0 ]; then
    err_L1 "EXIT FROM install.sh"
    exit
fi

exit 0

