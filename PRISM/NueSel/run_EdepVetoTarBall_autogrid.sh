#!/bin/bash

echo "Running on $(hostname) at ${GLIDEIN_Site}. GLIDEIN_DUNESite = ${GLIDEIN_DUNESite}"

# Set the output location for copyback
#OUTDIR=/pnfs/dune/persistent/users/${GRID_USER}/grid_jobs
OUTDIR=/pnfs/dune/scratch/users/${GRID_USER}/grid_jobs

# Make sure we see what we expect
echo "See where are at: pwd"
pwd

echo "tarball is copied and untarred at this worker node directory CONDOR_DIR_INPUT: ${CONDOR_DIR_INPUT}"

echo "ls -l CONDOR_DIR_INPUT: ${CONDOR_DIR_INPUT}"
# Tarball is copied and untarred into a directory on the worker node, accessed via this CONDOR_DIR_INPUT environment variable
ls -l $CONDOR_DIR_INPUT

echo "ls -l INPUT_TAR_DIR_LOCAL: ${INPUT_TAR_DIR_LOCAL} (should see .sh)"
ls -l $INPUT_TAR_DIR_LOCAL
#echo ${INPUT_TAR_DIR_LOCAL}

if [ -e ${INPUT_TAR_DIR_LOCAL}/setupEdepVetoTarBall-grid.sh ]; then
  echo "Start to run setupEdepVetoTarBall-grid.sh"
  . ${INPUT_TAR_DIR_LOCAL}/setupEdepVetoTarBall-grid.sh
  
  echo "Start to run larsoft_for_grid_setup.sh"
  source ${INPUT_TAR_DIR_LOCAL}/larsoft_for_grid_setup.sh # do this first to allow pip install 

  echo "which pip"
  which pip
else
  echo "Error, setup script not found. Exiting."
  exit 1
fi

echo "Finished run setupEdepVetoTarBall-grid.sh"

# Go back to the top-level directory since we know that's writable
echo "cd _CONDOR_JOB_IWD: ${_CONDOR_JOB_IWD}"
cd ${_CONDOR_JOB_IWD}

echo "pip install matplotlib"
pip install matplotlib
echo "pip install pandas"
pip install pandas
echo "pip install tables"
pip install tables

echo "ls -l _CONDOR_JOB_IWD/.local/lib/python3.9/site-packages"
ls -l ${_CONDOR_JOB_IWD}/.local/lib/python3.9/site-packages
export PYTHONPATH=${_CONDOR_JOB_IWD}/.local/lib/python3.9/site-packages:$PYTHONPATH 

# Symlink the desired file to the current directory
ln -s ${INPUT_TAR_DIR_LOCAL}/EdepVeto_forgrid_final.py .
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

# Loop over file list in txt file (samweb list-files "Dimensions")  #Eran make sure use one ending in ..._processed2.txt
for ifile in $(cat ${INPUT_TAR_DIR_LOCAL}/dune_nd_production_2022_v1_FHC_fiducial_nd_hall_with_lar_tms_sand_TDR_edep_LOCATIONS_processed2.txt | head -${LINE_N} | tail -1); do
  # Get the xrootd URL for the input file. Not necessary for SAM inputs when using ifdh_art, etc. #Eran don't need to do
  myinfile="${myinfile} ${ifile}"
#  myinfile="${myinfile} $(samweb get-file-access-url --schema=root ${ifile})"
done

echo "Got xrootd url: $myinfile"

echo "which python"
which python

# copy the input edepsim file to working dir becassue it can't read via root://fndca1.fnal.gov:1094
# this copy should be avoided
echo "ifdh cp -D $myinfile ${_CONDOR_JOB_IWD}"
ifdh cp -D $myinfile ${_CONDOR_JOB_IWD}

echo "ls -l _CONDOR_JOB_IWD"
ls -l ${_CONDOR_JOB_IWD}

#echo "python3 EdepVeto_forgrid_final.py $myinfile"
#python3 EdepVeto_forgrid_final.py $myinfile
echo "python3 EdepVeto_forgrid_final.py neutrino*.root"
python3 EdepVeto_forgrid_final.py neutrino*.root
LAR_RESULT=$?   # check the exit status!!!

if [ $LAR_RESULT -ne 0 ]; then
  echo "lar exited with abnormal status $LAR_RESULT. See error outputs."
  exit $LAR_RESULT
fi

echo "Have output"

# Unique name in case we send multiple jobs for each of seven outputs
OUTFILE=XYZPxPyPzveto_${CLUSTER}_${PROCESS}.h5

if [ -f XYZPxPyPzveto.h5 ]; then  #I think can do same loop for each file 

  echo "mv XYZPxPyPzveto.h5 $OUTFILE"
  mv XYZPxPyPzveto.h5 $OUTFILE

  # and copy our output file back , copying ALL seven files to local area
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
