#!/bin/sh

# INSTALL ONLY DESERT THINGS, IDEALLY ONLY THE FOLDER THAT I CARE ABOUT.

# INCLUDE
#. ./commonVariables.sh
. ${ROOT_DESERT}/commonFunctions.sh

if [ "${_DEBUG}" = "1" ]; then
    debug__print_screen_L1 "inside --- $0"
fi

# curr_ver="$(gcc --version | head -n1 | cut -d" " -f3)"
# last_ver="6.2.0"
# if [ "$(printf "$last_ver\n$curr_ver" | sort -V | head -n1)" = "$curr_ver" -a "$curr_ver" != "$last_ver" ]; then
#     export GCC6_FLAG=""
# else
#     export GCC6_FLAG="-std=c++98"
# fi

DIRFOLDER="${ZLIB_DIR}      \
           ${TCL_DIR}       \
           ${OTCL_DIR}      \
           ${TCLCL_DIR}     \
           ${NS_DIR}        \
           ${NSMIRACLE_DIR} \
           ${BELLHOP_DIR}   \
           ${NETCDF_DIR}    \
           ${NETCDFCXX_DIR} \
           ${WOSS_DIR}      \
          "

#for dirfolder in ${DIRFOLDER}; do
    #printf " * ${dirfolder}"
    #cp -r ${UNPACKED_FOLDER}/${dirfolder} ${DEST_FOLDER}/.buildHost
#done

main() {
    #******************************************************************************
    # MAIN
    #     e.g handle_package host/target <pkt-name>
    #     e.g addon_installation_list host/target <addon-list>

    ## only for the cross-compilation session
    export CROSS_ENV_DIR=""
    export CROSS_ENV_FILE=""
    export PATH="${BUILD_HOST}/bin:$PATH"
    export LD_LIBRARY_PATH="${BUILD_HOST}/lib"
    handle_package_modified host DESERT
    #******************************************************************************
}

#***
# << DESERT package >>
# -------
# This function allows the compilation/cross-compilation of the zlib package.
# Through the:
#    ${ARCH}
#    ${HOST}
# variables "build_DESERT ()" decides if do a compilation or a cross-compilation:
#    ${ARCH} = ${HOST}  -> compile
#    ${ARCH} != ${HOST} -> cross-compile

#TODO:
# (v) add the "return check" after each compile command. Moreover add "tail -n 50" command when a error compile happen.
#*
build_DESERT() {
    info_L1 "desert-${DESERT_VERSION}"
    start="$(date +%s)"

    (
        cd ${ROOT_DESERT}/${DESERT_DIR}
            ./autogen.sh >> /dev/null  2>&1
            ./autogen.sh >> "${currentBuildLog}/desert-${DESERT_VERSION}-$*.log"  2>&1
    )

    info_L2 "configure  [$*]"
    CXXFLAGS="-Wno-write-strings -std=c++11"                                                                  \
      CFLAGS="-Wno-write-strings"                                                                             \
    ${ROOT_DESERT}/${DESERT_DIR}/configure --target=${ARCH}                                                   \
                                           --host=${ARCH}                                                     \
                                           --build=${HOST}                                                    \
                                           --with-ns-allinone=${currentBuildLog}                              \
                                           --with-nsmiracle=${currentBuildLog}/nsmiracle-${NSMIRACLE_VERSION} \
                                           --prefix=${DEST_FOLDER}                                            \
                                           >> "${currentBuildLog}/desert-${DESERT_VERSION}-$*.log"  2>&1
    if [ $? -ne 0 ] ; then
        err_L1 "Error during the configuration of DESERT! Exiting ..."
        tail -n 50 ${currentBuildLog}/desert-${DESERT_VERSION}-$*.log
        exit 1
    fi
    info_L2 "patch      [$*]"
    (
        cat "${UNPACKED_FOLDER}/${PATCHES_DIR}/001-desert-2.0.0-libtool-no-verbose.patch" | patch -p1 >> "${currentBuildLog}/desert-${DESERT_VERSION}-$*.log"  2>&1
    )

    info_L2 "make       [$*]"
    make >> "${currentBuildLog}/desert-${DESERT_VERSION}-$*.log"  2>&1
    if [ $? -ne 0 ] ; then
        err_L1 "Error during the compilation of DESERT! Exiting ..."
        tail -n 50 ${currentBuildLog}/desert-${DESERT_VERSION}-$*.log
        exit 1
    fi

    info_L2 "make inst  [$*]"
    make install-strip >> "${currentBuildLog}/desert-${DESERT_VERSION}-$*.log"  2>&1
    if [ $? -ne 0 ] ; then
        err_L1 "Error during the installation of DESERT! Exiting ..."
        tail -n 50 ${currentBuildLog}/desert-${DESERT_VERSION}-$*.log
        exit 1
    fi

    elapsed=`expr $(date +%s) - $start`
    ok_L1 "completed in ${elapsed}s"
}

main

