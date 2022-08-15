#!/bin/bash

DIRECTORY=FDEff
# we cannot rely on "whoami" in a grid job. We have no idea what the local username will be.
# Use the GRID_USER environment variable instead (set automatically by jobsub).
USERNAME=${GRID_USER}
echo "Got username"
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup dunetpc v09_22_02 -q e19:debug # this also sets up the ifdh used in run_FDEffTarBall.sh
echo "dunetpc is set"
export WORKDIR=${_CONDOR_JOB_IWD} # if we use the RCDS the localProducts area will be placed in $CONDOR_DIR_INPUT
if [ ! -d "$WORKDIR" ]; then
  export WORKDIR=`echo ~`
fi


echo "Check work dir _CONDOR_JOB_IWD: ls -l ${_CONDOR_JOB_IWD}"
ls -l ${_CONDOR_JOB_IWD}

echo "Started set up grid"
echo "Check setup-grid exist: ls -l ${INPUT_TAR_DIR_LOCAL}/${DIRECTORY}/localProducts*"
ls -l ${INPUT_TAR_DIR_LOCAL}/${DIRECTORY}/localProducts*
source ${INPUT_TAR_DIR_LOCAL}/${DIRECTORY}/localProducts*/setup-grid
echo "Finished set up grid, now mrbsetenv"
mrbsetenv
#echo "Recompile"
#cd ${MRB_BUILDDIR}                        # Go to your build directory
#mrb z                                     # Remove old build directory
#mrbsetenv
echo "Done mrbsetenv"
#cd ${_CONDOR_JOB_IWD}                     # Go back to local work directory
