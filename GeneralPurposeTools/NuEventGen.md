# Instruction for Neutrino Event Generation

## Genie based on DUNE flux + edepsim setup 
Neutrino events generation with Genie and then run through edepsim for Geant4 simulation, pick a set of geometry and physics volume:
```
------------------------------------------------------------
| GEOMETRY  |  MPD_SPY_LAr.gdml    |  LArBath.gdml (default)
------------------------------------------------------------
| TOPVOL    |  volArgonCubeActive  |  volTPCActive
------------------------------------------------------------
```

Input can be number of events (default),
```
./GenieGen_Edepsim.sh 100
```
or POT,
```
# Enable this line: NEVENTS="-e ${NPOT}"
# NPOT 1e14: ~27 events
#      1e15: ~327 events
./GenieGen_Edepsim.sh 1e14
```

To change flavor,
```
set MODE inside GenieGen_Edepsim.sh:
"neutrino", "antineutrino"
```

## Edep-sim only setup

Simulate single particles in a detector geometry.

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

# Run options refer to https://github.com/ClarkMcGrew/edep-sim#running-the-detector-simulation
```
