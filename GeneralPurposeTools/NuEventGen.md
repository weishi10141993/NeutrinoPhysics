# Instruction for Neutrino Event Generation

## Monoenergetic neutrino event generation with Genie + edepsim setup

Usage: ```./GenieGen_MonoE_Flux_Edepsim.sh <energy in GeV> <neutrino PDG code> <number of events>```.

Target is default to Ar-40 (PDG: 1000180400).

```
# This generates 10 'CC' events of numu (PDG code: 14) scattered of Ar40 at an energy of 3GeV
./GenieGen_MonoE_Flux_Edepsim.sh 3 14 10

# It boils down to this command:
# gevgen -n 10 -e 3 -p 14 -t 1000180400 --seed 3258 -r 3258 --cross-sections ${GENIEXSECPATH}/gxspl-FNALsmall.xml
```

Refer to [Genie manual Sec. 7.3](https://genie-docdb.pp.rl.ac.uk/DocDB/0000/000002/007/man.pdf) for more options. The ```--event-generator-list``` [directive](https://github.com/GENIE-MC/Generator/blob/master/config/EventGeneratorListAssembler.xml)  is set to ```CC``` to only generate CC events, DUNE generally uses ```Default+CCMEC```.

For more info on Genie output ```rootracker``` format (```DetSimPassThru/gRooTracker``` in edepsim), refer to above manual Sec. 9.5.2.3.

For more options on scintillation simulation, refer to [edepsim commandlist](https://github.com/ClarkMcGrew/edep-sim/blob/master/doc/edepsim-command.list#L3800).

A simple macro to analyze the edepsim file:
```
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup edepsim v3_2_0 -q e20:prof

python3 Edepsim_ana.py edep.*.root
```

Another useful [event display tool](https://github.com/czczc/PyEdep/tree/main#description) by C. Zhang.

## DUNE neutrino flux event generation with Genie + edepsim setup

Pick a set of geometry and physics volume:
```
------------------------------------------------------------
| GEOMETRY  |  MPD_SPY_LAr.gdml    |  LArBath.gdml (default)
------------------------------------------------------------
| TOPVOL    |  volArgonCubeActive  |  volTPCActive
------------------------------------------------------------
```

Input can be number of events (default),
```
./GenieGen_DUNE_Flux_Edepsim.sh 100
```
or POT,
```
# Enable this line: NEVENTS="-e ${NPOT}"
# NPOT 1e14: ~27 events
#      1e15: ~327 events
./GenieGen_DUNE_Flux_Edepsim.sh 1e14
```

To change flavor,
```
set MODE inside GenieGen_DUNE_Flux_Edepsim.sh:
"neutrino", "antineutrino"
```

## Edep-sim only setup

Simulate single particles in a detector geometry under DUNE environment.

```
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup geant4 v4_10_6_p01e -q e20:prof
setup edepsim v3_2_0 -q e20:prof

# The following config file generates muons with energy of 10GeV: sim_inputs/Gen_muon.mac

edep-sim \
    -C \
    -g LArBath.gdml \
    -o edep.LArBath.muon.root \
    -u \
    -e 100 \
    Gen_muon.mac
```

It's also possible to build [standalone edepsim from source](https://github.com/ClarkMcGrew/edep-sim#running-the-detector-simulation).

## Detector Geometry

There are three options to generate desired geometry for event generation and simulation. Edepsim primarily takes gdml file as input.

### Option 1: Generate GDML with GeGeDe

DUNE Github has a generic repo called [duneggd](https://github.com/DUNE/duneggd) where a [larfd geometry template](https://github.com/DUNE/duneggd/tree/master/python/duneggd/larfd) is available. Another similar repo called [dunendggd](https://github.com/DUNE/dunendggd) but mostly used for ND. Both are created based on Brett's original [ggd](https://github.com/brettviren/gegede/).

The following sets up ```dunendggd``` to generate a desired geometry. It's adapted from [Ref](https://github.com/DUNE/dunendggd/tree/master#setup).

[First time only]
```
mkdir FD3Geo
cd FD3Geo

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup python v3_9_13

# The following gegede package needs these packages at ~/.local/lib/python3.9/site-packages: lxml, pint, nose
pip install lxml pint nose
pip install git+https://github.com/brettviren/gegede.git@86ca28190516a23203cd883aafb0548a61664ceb

git clone https://github.com/weishi10141993/dunendggd.git
cd dunendggd
python setup.py develop --user
export PATH=~/.local/bin/:${PATH}

# Test if the setup works, you should see a file created: nd_hall_no_dets.gdml
./build_hall.sh empty

# The above command is equivalent to running this:
gegede-cli duneggd/Config/WORLDggd.cfg \
	   duneggd/Config/ND_Hall_Air_Volume_NoDets.cfg \
	   duneggd/Config/ND_Hall_Rock.cfg \
	   duneggd/Config/ND_ElevatorStruct.cfg \
	   duneggd/Config/ND_CraneRailStruct1.cfg \
	   duneggd/Config/ND_CraneRailStruct2.cfg \
	   duneggd/Config/ND_HallwayStruct.cfg \
	   duneggd/Config/ND_CryoStruct.cfg \
	   -w World -o nd_hall_no_dets.gdml
```

Every time re-login to work, do these to set up:
```
cd dunendggd
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup python v3_9_13
python setup.py develop --user
export PATH=~/.local/bin/:${PATH}
```

### Option 2: Build Geometry into edepsim

It is possible to build C++ described geometry into edepsim source code following the example of [Captain](https://github.com/ClarkMcGrew/edep-sim/tree/master/src/captain) (was used to debug edepsim). Downside: need rebuild each time geometry is changed.

### Option 3: Generate GDML with perl script in LArSoft

Current DUNE FD geometries are generated by perl scripts which are available in ```dunecore/Geometry/gdml``` (e.g. [10kt fd v5](https://github.com/DUNE/dunecore/blob/develop/dunecore/Geometry/gdml/generate_dune10kt_v5_refactored.pl).

To use the perl script, you need to (install a few packages)[https://cdcvs.fnal.gov/redmine/projects/dunetpc/wiki/_Tips_on_how_to_regenerate_these_]. Best contact is L. Paulucci.

## Scintillation Light Simulation

As long as sensitive detectors are defined in GDML in auxiliary fields, the energy deposition in each sensitive detector is recorded in edepsim. Edepsim intentionally avoided light simulation in the beginning. You have access to hit segment SecondaryDeposit(```GetSecondaryDeposit()```) when the secondary deposit simulation is turned on. This is the tot energy deposited as optical photons. From this you can calculate the total number of photons, but simulation of the propogation of these photons is not possible in edepsim. The scintillation light simulation better comes from standalone GEANT4 or LArSoft (as HD and VD already have light sim built in).
