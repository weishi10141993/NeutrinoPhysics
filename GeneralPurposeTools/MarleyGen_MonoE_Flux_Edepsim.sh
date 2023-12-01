#!/bin/bash
shopt -s expand_aliases
# need expand alias for those defined in edepsim below to work

################################################################################
# Script to run marley + edepsim on dunegpvm
################################################################################
# Options

INPUTS_DIR="sim_inputs"
GEOMETRY="LArBath.gdml"
EDEP_MAC="marley-hepevt.mac"

ENERGY=$1
NEVENTS=$2
################################################################################

ls

# Don't try over and over again to copy a file when it isn't going to work
export IFDH_CP_UNLINK_ON_ERROR=1
export IFDH_CP_MAXRETRIES=1
export IFDH_DEBUG=0

# Dump the current clean environment to to env.sh so it can be restored when needed
echo "Saving environment env.sh"
declare -px > env.sh

# Setting up for gensim stuff
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup ifdhc v2_6_6
setup marley v1_2_0f -q e20:prof

cp ${INPUTS_DIR}/* .

# Copy template json config and modify
cp monoE.js monoE.${ENERGY}.MeV.nue.Evt.${NEVENTS}.js
sed -i "s/energy: 30/energy: $ENERGY/g" monoE.${ENERGY}.MeV.nue.Evt.${NEVENTS}.js
sed -i "s/events: 10/events: $NEVENTS/g" monoE.${ENERGY}.MeV.nue.Evt.${NEVENTS}.js

# Run Marley
echo "Running marley"
marley
marley monoE.${ENERGY}.MeV.nue.Evt.${NEVENTS}.js

# Prepare for edepsim
# cmake, geant4 and root required for edepsim to compile
setup cmake v3_19_6
setup geant4 v4_10_3_p03e -q e19:prof
setup root v6_18_04d -q e19:prof

# edep-sim needs to know where a certain GEANT .cmake file is...
G4_cmake_file=`find ${GEANT4_FQ_DIR}/lib64 -name 'Geant4Config.cmake'`
export Geant4_DIR=`dirname $G4_cmake_file`
# edep-sim needs to have the GEANT bin directory in the path
export PATH=$PATH:$GEANT4_FQ_DIR/bin

# Marley HEPEVT format is not supported in edepsim ```v3_2_0c``` in ups products.
#
# Instead, compile from source code using edepsim ```master``` branch:
# this fork has modified setup.sh
echo "git clone edep-sim"
# remove old code
rm -rf edep-sim
git clone https://github.com/weishi10141993/edep-sim.git

cd edep-sim
source setup.sh
edep-build

echo "Running edepsim"
cd ..
echo "edep-sim -C -g $GEOMETRY -o edep.${ENERGY}.MeV.nue.Evt.${NEVENTS}.root -e ${NEVENTS} ${EDEP_MAC}"
edep-sim -C \
         -g $GEOMETRY \
         -o edep.${ENERGY}.MeV.nue.Evt.${NEVENTS}.root \
         -e ${NEVENTS} \
         ${EDEP_MAC}
