# Instruction for Near to Far Detector Event Translation

## Set up dunendggd

This set up the software to generate a desired geometry for its use in edepsim or G4. It's adapted from [Ref](https://github.com/DUNE/dunendggd/tree/master#setup).

[First time only]
```
mkdir larbathdunendggd
cd larbathdunendggd

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup python v3_9_13
# The following gegede package needs these packages at ~/.local/lib/python3.9/site-packages: lxml, pint, nose
# you can install them using: pip install lxml pint nose
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

./build_hall.sh empty
```

ROOT TBrowser visualization:
```
TGeoManager::Import("test.gdml")
gGeoManager->GetTopVolume()->Draw("ogl");
```

## Set up edep-sim

Once obtained a GDML file (below use ```SIMPLECRYO.gdml``` as example), run edep-sim to simulate event energy deposits.

```
# Setup edep-sim
mkdir edepsim
cd edepsim

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup geant4 v4_10_6_p01e -q e20:prof
setup edepsim v3_2_0 -q e20:prof

# The following config file generates muons with energy of 10GeV
wget https://raw.githubusercontent.com/weishi10141993/NeutrinoPhysics/main/PRISM/N2FD/muon-10000.mac

edep-sim \
    -C \
    -g SIMPLECRYO.gdml \
    -o edep.SIMPLECRYO.root \
    -u \
    -e 100 \
    muon-10000.mac

# Run options refer to https://github.com/ClarkMcGrew/edep-sim#running-the-detector-simulation
```

## Analyze and validate the edepsim file

```
mkdir edepsimana
cd edepsimana

wget https://raw.githubusercontent.com/weishi10141993/NeutrinoPhysics/main/PRISM/N2FD/setup.sh
source setup.sh

wget https://raw.githubusercontent.com/weishi10141993/NeutrinoPhysics/main/PRISM/N2FD/Edepsim_ana.py
python3 Edepsim_ana.py edep.SIMPLECRYO.root

# The first time you may need to install a few packages via pip install, e.g., pip install matplotlib pandas
# depending on what it complains when you run: python3 Edepsim_ana.py edep.SIMPLECRYO.root
```
