#!/bin/bash

echo "Running on $(hostname) at ${GLIDEIN_Site}. GLIDEIN_DUNESite = ${GLIDEIN_DUNESite}"

# Set the output location for copyback, persistent not writable
OUTDIR=/pnfs/dune/scratch/users/${GRID_USER}/FDCAFlegacyGECprod/nu

# Make sure we see what we expect
echo "See where are at: pwd" # this normally is _CONDOR_JOB_IWD
pwd

echo "tarball is copied and untarred at this worker node directory CONDOR_DIR_INPUT: ${CONDOR_DIR_INPUT}"

echo "ls -l CONDOR_DIR_INPUT"
# Tarball is copied and untarred into a directory on the worker node, accessed via this CONDOR_DIR_INPUT environment variable
ls -l $CONDOR_DIR_INPUT

echo "ls -l INPUT_TAR_DIR_LOCAL: ${INPUT_TAR_DIR_LOCAL} (should see .sh and the untarred dunetpclegacy folder)"
ls -l $INPUT_TAR_DIR_LOCAL

if [ -e ${INPUT_TAR_DIR_LOCAL}/setup_FDlegacyCAF-grid.sh ]; then
  echo "Start to run setup_FDlegacyCAF-grid.sh"
  . ${INPUT_TAR_DIR_LOCAL}/setup_FDlegacyCAF-grid.sh
else
  echo "Error, setup script not found. Exiting."
  exit 1
fi

echo "Finished run setup_FDlegacyCAF-grid.sh"

# Go back to the top-level directory since we know that's writable
echo "cd _CONDOR_JOB_IWD: ${_CONDOR_JOB_IWD}"
cd ${_CONDOR_JOB_IWD}

echo "ls _CONDOR_JOB_IWD"
ls
echo "And ls _CONDOR_DIR_INPUT: ${_CONDOR_DIR_INPUT}"
ls ${_CONDOR_DIR_INPUT}

# Set some other very useful environment variables for xrootd and IFDH
export IFDH_CP_MAXRETRIES=2
export XRD_CONNECTIONRETRY=32
export XRD_REQUESTTIMEOUT=14400
export XRD_REDIRECTLIMIT=255
export XRD_LOADBALANCERTTL=7200
export XRD_STREAMTIMEOUT=14400 # may vary for your job/file type

# Make sure the output directory exists
# Do not do ifdh ls on grid!!!
# Can cause system-wide slowdowns, especially if the directory has too many files in it
#ifdh ls $OUTDIR 0 # set recursion depth to 0 since we are only checking for the directory; we don't care about the full listing.

if [ $? -ne 0 ]; then
  # if ifdh ls failed, try to make the directory
  ifdh mkdir_p $OUTDIR || { echo "Error creating or checking $OUTDIR"; exit 2; }
fi

echo "Finished checking outdir: $OUTDIR"

myinfile=""
# PROCESS starts from 0, 1, ... N-1
(( LINE_N = ${PROCESS} + 1 ))

# Loop over file list in txt file (samweb list-files "Dimensions")
for ifile in $(cat ${INPUT_TAR_DIR_LOCAL}/MCC11FDBeamsim_nu_reco.txt | head -${LINE_N} | tail -1); do
  # Get the xrootd URL for the input file. Not necessary for SAM inputs when using ifdh_art, etc.
  myinfile="${myinfile} $(samweb get-file-access-url --schema=root ${ifile})"
done

echo "Got xrootd url: $myinfile"

# Now we should be in the work dir if setup_FDlegacyCAF-grid.sh worked
cd ${_CONDOR_JOB_IWD}/${DIRECTORY}/srcs/dunetpc/dune/CAFMaker

echo "echo PYTHONPATH: "
echo $PYTHONPATH
export PYTHONPATH=${PWD}/DUNE_ND_GeoEff/lib:${PYTHONPATH}
echo "echo PYTHONPATH again "
echo $PYTHONPATH

echo "ls -l PWD"
ls -l ${PWD}

echo "echo LD_LIBRARY_PATH: "
echo $LD_LIBRARY_PATH
# cmake doesn't know some paths if it's compiled on a machine the grid doesn't know
# add to LD_LIBRARY_PATH:
#   path to generated dictionaries for nested vectors: ${PWD}
#   path to libgeoEff: ${PWD}/DUNE_ND_GeoEff/lib
#   path to commonly used libs under /lib64, also append at the very end to avoid version issues.
#     On gpvm they exist under /lib64, but not available on grid node: ${INPUT_TAR_DIR_LOCAL}/lib64
# PWD should be /dunetpc/dune/CAFMaker, this is where nested vectors' dictionaries are located if the program is ran interactively
echo "export LD_LIBRARY_PATH=PWD/DUNE_ND_GeoEff/lib:LD_LIBRARY_PATH:INPUT_TAR_DIR_LOCAL/lib64:PWD"
export LD_LIBRARY_PATH=${PWD}/DUNE_ND_GeoEff/lib:${LD_LIBRARY_PATH}:${INPUT_TAR_DIR_LOCAL}/lib64:${PWD}
echo "echo LD_LIBRARY_PATH again: "
echo $LD_LIBRARY_PATH

echo "lar -c ../../fcl/dunefd/mergeana/select_ana_dune10kt_nu.fcl -n -1 $myinfile"
lar -c ../../fcl/dunefd/mergeana/select_ana_dune10kt_nu.fcl -n -1 $myinfile
LAR_RESULT=$?   # check the exit status!!!

echo "ls -l PWD (check dictionaries renewed?)"
ls -l ${PWD}

if [ $LAR_RESULT -ne 0 ]; then
  echo "lar exited with abnormal status $LAR_RESULT. See error outputs."
  exit $LAR_RESULT
fi

echo "Have output"
echo "ls to see if we can see the output file" # this normally is _CONDOR_JOB_IWD
ls

# Unique name in case we send multiple jobs.
OUTFILE=fdcaf_legacy_${CLUSTER}_${PROCESS}.root

if [ -f caf_hist.root ]; then

  echo "mv caf_hist.root $OUTFILE"
  mv caf_hist.root $OUTFILE

  # and copy our output file back
  ifdh cp -D $OUTFILE $OUTDIR

  # check the exit status to see if the copyback actually worked. Print a message if it did not.
  IFDH_RESULT=$?
  if [ $IFDH_RESULT -ne 0 ]; then
    echo "Error during output copyback. See output logs."
    exit $IFDH_RESULT
  fi
fi

#If we got this far, we succeeded.
echo "Completed successfully."
exit 0
