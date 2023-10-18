#!/bin/bash
################################################################################
# Script to run genie + edepsim
# Adapted from legacy DUNE ND CAFMaker
################################################################################
# Options

INPUTS_DIR="sim_inputs"

GEOMETRY="LArBath.gdml"
TOPVOL="volTPCActive"
EDEP_MAC="dune-fd3.mac"

ENERGY=$1
NUPDG=$2
NEVENTS=$3

################################################################################

RNDSEED=$RANDOM
RNDSEED=`echo "$RNDSEED" | cut -f 1 -d '.'`

ls

# Don't try over and over again to copy a file when it isn't going to work
export IFDH_CP_UNLINK_ON_ERROR=1
export IFDH_CP_MAXRETRIES=1
export IFDH_DEBUG=0

# Dump the current clean environment to to env.sh so it can be restored when needed
echo "Saving environment env.sh"
declare -px > env.sh

# Setting up for genie stuff
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup ifdhc v2_6_6
# dk2nugenie automatically sets genie
setup dk2nugenie   v01_06_01f -q debug:e15
# genie_xsec defines which "tune" you're going to use
setup genie_xsec   v2_12_10   -q DefaultPlusValenciaMEC
# genie_phyopt sets some overrides to ensure charm and tau get decayed "properly"
setup genie_phyopt v2_12_10   -q dkcharmtau
setup geant4 v4_10_3_p01b -q e15:prof

# edep-sim needs to know where a certain GEANT .cmake file is...
G4_cmake_file=`find ${GEANT4_FQ_DIR}/lib64 -name 'Geant4Config.cmake'`
export Geant4_DIR=`dirname $G4_cmake_file`

# edep-sim needs to have the GEANT bin directory in the path
export PATH=$PATH:$GEANT4_FQ_DIR/bin

cp ${INPUTS_DIR}/* .

# Run GENIE
echo "Running gevgen"
gevgen \
    -e ${ENERGY} \
    -n ${NEVENTS} \
    -p ${NUPDG} \
    -t 1000180400 \
    --seed ${RNDSEED} \
    -r ${RNDSEED} \
    --message-thresholds Messenger_production.xml \
    --cross-sections ${GENIEXSECPATH}/gxspl-FNALsmall.xml \
    --event-record-print-level 0 \
    --event-generator-list CC

# Convert the genie output to rootracker
echo "Running gntpc"
cp gntp.${RNDSEED}.ghep.root input_file.ghep.root
gntpc -i input_file.ghep.root -f rootracker \
      --event-record-print-level 0 \
      --message-thresholds Messenger_production.xml

# edep-sim wants number of events, but we are doing POT so the files will be slightly different
# get the number of events from the GENIE files to pass it to edep-sim
NPER=$(echo "std::cout << gtree->GetEntries() << std::endl;" | \
       genie -l -b input_file.ghep.root 2>/dev/null | \
       tail -1)

setup edepsim v3_2_0 -q e20:prof

echo "Running edepsim"
edep-sim -C \
         -g $GEOMETRY \
         -o edep.${RNDSEED}.root \
         -e ${NPER} \
         ${EDEP_MAC}
