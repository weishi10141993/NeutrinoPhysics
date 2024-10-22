# Instruction for MeV Neutrino Event Generation

## Monoenergetic neutrino event generation with Marley + edepsim setup

This section provides a single script to do both Marley event generation and edepsim. If you only need one of them, read the remaining two sections.

Usage: ```./MarleyGen_MonoE_Flux_Edepsim.sh <energy in MeV> <number of events>```.

Neutrino is default to nue. Target is default to Ar-40.

```
# To work on dunegpvm, set up SL7 container:

/cvmfs/oasis.opensciencegrid.org/mis/apptainer/current/bin/apptainer shell --shell=/bin/bash \
-B /cvmfs,/exp,/nashome,/pnfs/dune,/opt,/run/user,/etc/hostname,/etc/hosts,/etc/krb5.conf --ipc --pid \
/cvmfs/singularity.opensciencegrid.org/fermilab/fnal-dev-sl7:latest

export UPS_OVERRIDE="-H Linux64bit+3.10-2.17"

# For example, this generates 10 'CC' events of nue scattered of Ar40 at an energy of 20MeV
./MarleyGen_MonoE_Flux_Edepsim.sh 20 10

# Example events available: /pnfs/dune/persistent/users/weishi/FD3/LArBath/Marley_Edepsim_noSecondaryDeposit/nue
```

More configuations available on [Marley website](https://www.marleygen.org/index.html).

## [skip this section] Monoenergetic neutrino event generation with Marley

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

## [skip this section] edepsim setup to read Marley events

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

# Marley HEPEVT format is not supported in edepsim ```v3_2_0c``` in ups products.
#
# Instead, compile from source code using edepsim ```master``` branch:
# this fork has modified setup.sh
git clone https://github.com/weishi10141993/edep-sim.git

cd edep-sim
source setup.sh
edep-build

cd ..
edep-sim -C \
         -g LArBath.gdml \
         -o edep.LArBath.MeVnue.root \
         -e 10 \
         marley-hepevt.mac
```

## Analysis and Event visualization

This is a useful [event display tool](https://github.com/czczc/PyEdep?tab=readme-ov-file#description).

```
[First time only ]
# log in dunegpvm
git clone https://github.com/weishi10141993/PyEdep.git -b Marley
```

Work in dunegpvm (default to alma9), set SL7 container environment
```
/cvmfs/oasis.opensciencegrid.org/mis/apptainer/current/bin/apptainer shell --shell=/bin/bash \
-B /cvmfs,/exp,/nashome,/pnfs/dune,/opt,/run/user,/etc/hostname,/etc/hosts,/etc/krb5.conf --ipc --pid \
/cvmfs/singularity.opensciencegrid.org/fermilab/fnal-dev-sl7:latest

export UPS_OVERRIDE="-H Linux64bit+3.10-2.17"

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup edepsim v3_2_0 -q e20:prof

# [First time only: install matplotlib]
# Passing parameters norm and vmin/vmax simultaneously is deprecated since matplotlib 3.3
# change the target dir to your own dir
pip install --force-reinstall --target=/exp/dune/app/users/weishi/ChaoEvtVisuallibs "matplotlib==3.2.2"

export PYTHONPATH=/exp/dune/app/users/weishi/ChaoEvtVisuallibs:$PYTHONPATH

# Save a few event displays
python test.py

# Read Geant4 sim file and Output file with energy deposit info
python3 writer.py 'edep_nue_*.root' 'Marley' output_file.root
```
