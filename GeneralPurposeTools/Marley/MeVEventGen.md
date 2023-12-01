# Instruction for MeV Neutrino Event Generation

## Monoenergetic neutrino event generation with Marley + edepsim setup

This section provides a single script to do both Marley event generation and edepsim. If you only want one of them, read the remaining two sections.


## Monoenergetic neutrino event generation with Marley

Following works on dunegpvm:

```
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup marley v1_2_0f -q e20:prof

marley
# it prints the following:
  Usage: marley [OPTION...] CONFIG_FILE

    -h, --help     Print this help message
    -v, --version  Print version and exit

  MARLEY home page: <http://www.marleygen.org>
  E-mail bug reports to: <support@marleygen.org>

# Generate monoenergetic events
marley monoE.js
```

## edepsim setup to read Marley events

Marley HEPEVT format is not supported in edepsim ```v3_2_0c``` in ups products.

Instead, compile from source code using edepsim ```master``` branch:
```
# edepsim needs root and geant4 to compile
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup cmake v3_19_6
setup geant4 v4_10_3_p03e -q e19:prof
setup root v6_18_04d -q e19:prof

# edep-sim needs to know where a certain GEANT .cmake file is...
G4_cmake_file=`find ${GEANT4_FQ_DIR}/lib64 -name 'Geant4Config.cmake'`
export Geant4_DIR=`dirname $G4_cmake_file`
# edep-sim needs to have the GEANT bin directory in the path
export PATH=$PATH:$GEANT4_FQ_DIR/bin

# this fork has modified setup.sh
git clone https://github.com/weishi10141993/edep-sim.git

source setup.sh
edep-build

edep-sim -C \
         -g LArBath.gdml \
         -o edep.LArBath.MeVnue.root \
         -e 10 \
         marley-hepevt.mac
```
