#!/bin/bash


# we cannot rely on "whoami" in a grid job. We have no idea what the local username will be.
# Use the GRID_USER environment variable instead (set automatically by jobsub).
USERNAME=${GRID_USER}
echo "Got username"
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
#setup dunetpc v09_41_00_02 -q e20:debug # this also sets up the ifdh used in run_FDEffTarBall.sh
#echo "dunetpc is set"
export WORKDIR=${_CONDOR_JOB_IWD} # if we use the RCDS the localProducts area will be placed in $CONDOR_DIR_INPUT
if [ ! -d "$WORKDIR" ]; then
  export WORKDIR=`echo ~`
fi

# _CONDOR_JOB_IWD is /srv
echo "Check work dir _CONDOR_JOB_IWD: ls -l ${_CONDOR_JOB_IWD}"
ls -l ${_CONDOR_JOB_IWD}

# Copy the untarred folder and remove the CVMFS linked read-only version
# because later mrbsetenv needs to modify larsoft-v09_22_02 in build* directory
# which can't be done in the CVMFS read-only version

# INPUT_TAR_DIR_LOCAL is /srv/no_xfer/0/TRANSFERRED_INPUT_FILES/FDEff
# copy FDEff to local work dir _CONDOR_JOB_IWD: /srv
# now is /srv/FDEff/srcs

#echo "Check setup-grid exist and source it: ls -l ${_CONDOR_JOB_IWD}/${DIRECTORY}/localProducts*"
#ls -l ${_CONDOR_JOB_IWD}/${DIRECTORY}/localProducts*
#source ${_CONDOR_JOB_IWD}/${DIRECTORY}/localProducts*/setup-grid
#
#echo "Finished set up grid, now mrbsetenv"
#mrbsetenv
#echo "Finish mrbsetenv"
