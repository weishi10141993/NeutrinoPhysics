#!/bin/bash

echo "Running on $(hostname) at ${GLIDEIN_Site}. GLIDEIN_DUNESite = ${GLIDEIN_DUNESite}"

# Set the output location for copyback
#OUTDIR=/pnfs/dune/persistent/users/${GRID_USER}/myFDntuples
OUTDIR=/pnfs/dune/scratch/users/${GRID_USER}/files4baron

# Make sure we see what we expect
echo "See where are at: pwd" # this normally is _CONDOR_JOB_IWD
pwd

echo "tarball is copied and untarred at this worker node directory CONDOR_DIR_INPUT: ${CONDOR_DIR_INPUT}"

echo "ls -l CONDOR_DIR_INPUT"
# Tarball is copied and untarred into a directory on the worker node, accessed via this CONDOR_DIR_INPUT environment variable
ls -l $CONDOR_DIR_INPUT

echo "ls -l INPUT_TAR_DIR_LOCAL: ${INPUT_TAR_DIR_LOCAL} (should see untarred files)"
ls -l $INPUT_TAR_DIR_LOCAL

if [ -e ${INPUT_TAR_DIR_LOCAL}/setup_NDcombEff.sh ]; then
  echo "Start to run setup_NDcombEff.sh"
  . ${INPUT_TAR_DIR_LOCAL}/setup_NDcombEff.sh
else
  echo "Error, setup script not found. Exiting."
  exit 1
fi

echo "Finished run setupFDEffTarBall-grid.sh"

# Go back to the top-level directory since we know that's writable
echo "cd _CONDOR_JOB_IWD: ${_CONDOR_JOB_IWD}"
cd ${_CONDOR_JOB_IWD}

echo "Install dependencies"
echo "pip install --target=${_CONDOR_JOB_IWD} uproot4"
pip install --target=${_CONDOR_JOB_IWD} uproot4
echo "pip install --target=${_CONDOR_JOB_IWD} uproot3"
pip install --target=${_CONDOR_JOB_IWD} uproot3
echo "pip install --target=${_CONDOR_JOB_IWD} torch"
pip install --target=${_CONDOR_JOB_IWD} torch
echo "pip install --target=${_CONDOR_JOB_IWD} scipy"
pip install --target=${_CONDOR_JOB_IWD} scipy

export PYTHONPATH=${_CONDOR_JOB_IWD}:$PYTHONPATH

echo "ls -l _CONDOR_JOB_IWD"
ls -l ${_CONDOR_JOB_IWD}

# Symlink the desired file to the current directory
ln -s ${INPUT_TAR_DIR_LOCAL}/new_hadron_muon_mktree.py .
ln -s ${INPUT_TAR_DIR_LOCAL}/muonEff30.nn .
ln -s ${INPUT_TAR_DIR_LOCAL}/muonEffModel.py .
#echo "Did the symlink"

# Set some other very useful environment variables for xrootd and IFDH
export IFDH_CP_MAXRETRIES=2
export XRD_CONNECTIONRETRY=32
export XRD_REQUESTTIMEOUT=14400
export XRD_REDIRECTLIMIT=255
export XRD_LOADBALANCERTTL=7200
export XRD_STREAMTIMEOUT=14400 # may vary for your job/file type

# Make sure the output directory exists
ifdh ls $OUTDIR 0 # set recursion depth to 0 since we are only checking for the directory; we don't care about the full listing.

if [ $? -ne 0 ]; then
  # if ifdh ls failed, try to make the directory
  ifdh mkdir_p $OUTDIR || { echo "Error creating or checking $OUTDIR"; exit 2; }
fi

echo "Finished checking outdir: $OUTDIR"

myinfile=""
# PROCESS starts from 0, 1, ... N-1
(( LINE_N = ${PROCESS} + 1 ))

# Loop over file list in txt file
for ifile in $(cat ${INPUT_TAR_DIR_LOCAL}/NDCAFs.txt | head -${LINE_N} | tail -1); do
  myinfile="${myinfile} ${ifile}"
done

echo "Got xrootd url: $myinfile"

# copy the input file to working dir becassue python3 can't read via root://fndca1.fnal.gov:1094
# this copy should be avoided however for large size files
echo "ifdh cp -D $myinfile ${_CONDOR_JOB_IWD}"
ifdh cp -D $myinfile ${_CONDOR_JOB_IWD}

echo "ls -l _CONDOR_JOB_IWD"
ls -l ${_CONDOR_JOB_IWD}

echo "python3 new_hadron_muon_mktree.py ./*CAF.root"
python3 new_hadron_muon_mktree.py ./*CAF.root
LAR_RESULT=$?   # check the exit status!!!

if [ $LAR_RESULT -ne 0 ]; then
  echo "lar exited with abnormal status $LAR_RESULT. See error outputs."
  exit $LAR_RESULT
fi

echo "Have output"

echo "ls -l _CONDOR_JOB_IWD"
ls -l ${_CONDOR_JOB_IWD}

# Unique name in case we send multiple jobs.
#OUTFILE=myntuple_${CLUSTER}_${PROCESS}.root

if [ -f *Eff.root ]; then

  echo "Found output root file, now copy to scratch area: $OUTDIR"

  #echo "mv myntuple.root $OUTFILE"
  #mv myntuple.root $OUTFILE

  # and copy our output file back
  ifdh cp -D *Eff.root $OUTDIR

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
