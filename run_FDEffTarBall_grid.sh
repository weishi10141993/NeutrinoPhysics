#!/bin/bash

echo "Running on $(hostname) at ${GLIDEIN_Site}. GLIDEIN_DUNESite = ${GLIDEIN_DUNESite}"

# Set the number of input files using line number in txt file
# Require: STARTLINE <= ENDLINE
STARTLINE=2
ENDLINE=4
((TOTALIN = ${ENDLINE} - ${STARTLINE} + 1 ))
echo "Input file start line ${STARTLINE}, end line ${ENDLINE}, total files: ${TOTALIN}"

# Set the output location for copyback
OUTDIR=/pnfs/dune/persistent/users/${GRID_USER}/myFDntuples

# Let's rename the output file so it's unique in case we send multiple jobs.
OUTFILE=myntuple_${CLUSTER}_${PROCESS}_$(date -u +%Y%m%dT%H%M%SZ).root

# Make sure we see what we expect
echo "See where are at: pwd"
pwd

echo "tarball is copied and untarred at this worker node directory CONDOR_DIR_INPUT: ${CONDOR_DIR_INPUT}"

echo "ls -l CONDOR_DIR_INPUT"
# Tarball is copied and untarred into a directory on the worker node, accessed via this CONDOR_DIR_INPUT environment variable
ls -l $CONDOR_DIR_INPUT

echo "ls -l INPUT_TAR_DIR_LOCAL: ${INPUT_TAR_DIR_LOCAL} (should see .sh and the untarred FDEff folder)"
ls -l $INPUT_TAR_DIR_LOCAL

sleep 2

if [ -e ${INPUT_TAR_DIR_LOCAL}/setupFDEffTarBall-grid.sh ]; then
  echo "Start to run setupFDEffTarBall-grid.sh"
  . ${INPUT_TAR_DIR_LOCAL}/setupFDEffTarBall-grid.sh
else
  echo "Error, setup script not found. Exiting."
  exit 1
fi

echo "Finished run setupFDEffTarBall-grid.sh"

# Go back to the top-level directory since we know that's writable
echo "cd _CONDOR_JOB_IWD: ${_CONDOR_JOB_IWD}"
cd ${_CONDOR_JOB_IWD}

echo "ls _CONDOR_JOB_IWD"
ls
echo "And ls _CONDOR_DIR_INPUT: ${_CONDOR_DIR_INPUT}"
ls ${_CONDOR_DIR_INPUT}

# Symlink the desired fcl to the current directory
ln -s ${INPUT_TAR_DIR_LOCAL}/${DIRECTORY}/srcs/myntuples/myntuples/MyEnergyAnalysis/MyEnergyAnalysis.fcl .
echo "Did the symlink"

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

# Loop over file list in txt file (samweb list-files "Dimensions")
for ifile in $(cat ${INPUT_TAR_DIR_LOCAL}/MCC11FDBeamsim_nu_reco.txt | sed -n ${STARTLINE},${ENDLINE}p); do
  # Get the xrootd URL for the input file. Not necessary for SAM inputs when using ifdh_art, etc.
  myinfile="${myinfile} $(samweb get-file-access-url --schema=root ${ifile})"
done

echo "Got xrootd urls: $myinfile"

# Now we should be in the work dir if setupFDEffTarBall-grid.sh worked
echo "lar -c MyEnergyAnalysis.fcl -n -1 $myinfile"
lar -c MyEnergyAnalysis.fcl -n -1 $myinfile
LAR_RESULT=$?   # check the exit status!!!

if [ $LAR_RESULT -ne 0 ]; then
  echo "lar exited with abnormal status $LAR_RESULT. See error outputs."
  exit $LAR_RESULT
fi

echo "Have output"

if [ -f myntuple.root ]; then

  echo "mv myntuple.root $OUTFILE"
  mv myntuple.root $OUTFILE

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
