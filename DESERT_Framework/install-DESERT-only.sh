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


# Terminal parameters check
if [ $# -eq 0 ]; then
    install__print_help

    logERR "[Terminal_parameter_check]: exit_1" ${INSTALL_LOG}
    exit 1
fi

#------

#Getopt setting --
shortOpt="abcd:e:f:g:hi:lm"
longOpt="wizard,\
         with-woss,\
         without-woss,\
         target:,\
         inst_mode:,\
         dest_folder:,\
         custom_par:,\
         help,\
         addons:,\
         wizard-conf,\
         wizard-asOwner"
ARGS=$(getopt -o $shortOpt   \
              -l "$longOpt"  \
              -n "install.sh" \
              -- "$@");
RETOPT_GETOPT=$?
if [ "${_DEBUG}" = "1" ]; then
    debug__print_screen_L1 "RETURN CODE of GETOPT = ${RETOPT_GETOPT}"
fi

if [ "${_DEBUG}" = "1" ]; then
    debug__print_screen_L1 "AFTER GETOPT: \t\t$@"
fi

#Bad arguments after getopt --
if [ ${RETOPT_GETOPT} -ne 0 ]; then
    install__print_help
    logERR "some parameters are not correct" ${INSTALL_LOG}
    exit 1
fi

#Getopt setting --
eval set -- "$ARGS";
if [ "${_DEBUG}" = "1" ]; then
    debug__print_screen_L1 "AFTER SET -- \$ARGS: \t$@"
fi
while true; do
    case "$1" in
        -a|--wizard)
            shift;
            OWNER_PERMISSION=0
            ADDONS_FILE=".addon.list"
            _WIZARD_OPT=1
            log_L1 "_WIZARD_OPT=${_WIZARD_OPT}" ${INSTALL_LOG}
            if [ -n "$1" ]; then
                if [ "${_DEBUG}" = "1" ]; then
                    debug__print_screen_L1 "_WIZARD_OPT=${_WIZARD_OPT}"
                    debug__print_screen_L1 "OWNER_PERMISSION=${OWNER_PERMISSION}"
                    debug__print_screen_L1 "no parameter for --wizard option"
                fi
                #shift
            fi
            DEST_FOLDER="/home/chris/Desktop/DESERT_Underwater/DESERT_buildCopy_LOCAL"
            BUILD_HOST="${DEST_FOLDER}/.buildHost"
            BUILD_TARGET="${DEST_FOLDER}/.buildTarget"
            TARGET="LOCAL"
            INST_MODE="development"
            BUILD_HOST="/home/chris/Desktop/DESERT_Underwater/DESERT_buildCopy_LOCAL/.buildHost"
            BUILD_TARGET="/home/chris/Desktop/DESERT_Underwater/DESERT_buildCopy_LOCAL/.buildTarget"
            CUSTOM_PAR=""
            ADDONS=""
            break;
            ;;
    esac
done

#***
# internal settings
#
#*

if [ "${_DEBUG}" = "1" ]; then
    debug__print_parameters
fi

info_L0 "make_dir"
make_dir_modified
if [ $? -ne 0 ]; then
    err_L1 "EXIT FROM install.sh"
    exit
fi

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

