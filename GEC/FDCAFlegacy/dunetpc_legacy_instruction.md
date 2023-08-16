# Run FD CAFMaker dunetpc legacy code (TDR version FD CAF)

## Setup

```
mkdir dunetpclegacy
cd dunetpclegacy

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh

setup larsoft v07_09_00 -q e17:prof
unsetup mrb
setup mrb v4_04_06          # need this version
mrb newDev
source /dune/app/users/weishi/dunetpclegacy/localProducts_larsoft_v07_09_00_e17_prof/setup

cd srcs

# On local laptop:
#   git clone https://github.com/weishi10141993/dunetpc.git -b feature/v07_09_00
# sync the changes in CAFMaker
cd ${LAPTOP_WORK_DIR}/dunetpc
rsync -e ssh -avSz  ./* weishi@dunegpvm13.fnal.gov:/dune/app/users/weishi/dunetpclegacy/srcs/dunetpc

# Build GEC code inside CAFMaker
cd dunetpc/dune/CAFMaker
./build.sh

# Go back to srcs and get systematics software
cd srcs    
mrb g nusystematics
cd nusystematics
git checkout remotes/origin/TDRSensProd
# Change this line in ups/product_deps: nutools v2_24_07 to nutools v2_24_05

cd srcs
mrb g systematicstools
cd systematicstools
git checkout remotes/origin/TDRSensProd

# Build the code:
cd srcs
mrb uc
cd ${MRB_BUILDDIR}       
mrb z
mrbsetenv
unsetup cmake
setup cmake v3_19_6
mrb b
```

Run legacy FD CAF maker production interactively,

```
lar -c ../../fcl/dunefd/mergeana/select_ana_dune10kt_nu.fcl -n 100 root://fndca1.fnal.gov:1094/pnfs/fnal.gov/usr/dune/tape_backed/dunepro/mcc11/protodune/mc/full-reconstructed/07/51/31/11/nu_dune10kt_1x2x6_13009312_0_20181104T221530_gen_g4_detsim_reco.root

# or in screen mode:
nohup lar -c ../../fcl/dunefd/mergeana/select_ana_dune10kt_nu.fcl -T caf_legacy_10.root -n -1 root://fndca1.fnal.gov:1094/pnfs/fnal.gov/usr/dune/tape_backed/dunepro/mcc11/protodune/mc/full-reconstructed/07/86/27/57/nu_dune10kt_1x2x6_13027311_0_20181110T041119_gen_g4_detsim_reco.root >& output10.log &
```

Recompile if source code changed,

```
cd ${MRB_BUILDDIR}                   
mrb z
mrbsetenv  
unsetup cmake
setup cmake v3_19_6
mrb b
```

Relogin,

```
cd /dune/app/users/weishi/dunetpclegacy
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup larsoft v07_09_00 -q e17:prof
unsetup mrb
setup mrb v4_04_06
source /dune/app/users/weishi/dunetpclegacy/localProducts_larsoft_v07_09_00_e17_prof/setup
mrbsetenv
```
