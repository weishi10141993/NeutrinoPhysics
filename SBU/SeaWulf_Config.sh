#!/bin/bash

# Assume you will run the first 180 files, 6 jobs, each job will run 30 files
(( LINE_N = 30 ))
for JOBNUM in 1 2 3 4 5 6
do
  myinfile=""
  # Loop over file list in txt file
  for ifile in $(cat MCC11FDBeamsim_nu_reco.txt | head -${LINE_N} | tail -30); do
    # Get the xrootd URL for the input file. Not necessary for SAM inputs when using ifdh_art, etc.
    myinfile="${myinfile} ${ifile}"
  done
  echo "Job number: ${JOBNUM} - myinfile: ${myinfile}"
  cp SeaWulf_Template.sh SeaWulf_Job${JOBNUM}.sh
  sed -i "s/myjobnumplaceholder/${JOBNUM}/g" SeaWulf_Job${JOBNUM}.sh
  sed -i "s/myinfileplaceholder/${myinfile}/g" SeaWulf_Job${JOBNUM}.sh
  sbatch SeaWulf_Job${JOBNUM}.sh
  (( LINE_N = LINE_N + 30 ))
  sleep 5
done
