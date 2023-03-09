# Integrate FD CAF maker with PRISM GEC FD code from DUNE FNAL machines (dunegpvm*)

## Setup using duneana develop branch

```
mkdir FDCAFIntegration
cd FDCAFIntegration

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh

# The dunesw version below is what ups says in the develop branch
# it may change as the develop branches update
# So may need to constantly update the version
# Is there an easier way so that we can avoid setting this version? ==> ASK Tom
setup dunesw v09_68_00d00 -q e20:debug  

mrb newDev
source /dune/app/users/weishi/FDCAFIntegration/localProducts_larsoft_v09_68_00_debug_e20/setup

# Get FD maker code
cd /dune/app/users/weishi/FDCAFIntegration/srcs

# On local laptop, sync the changes in CAFMaker
# After the local changes are pushed, replace with:
#   git clone https://github.com/weishi10141993/duneana.git -b develop
cd ${LAPTOP_WORK_DIR}/duneana
rsync -e ssh -avSz  ./* weishi@dunegpvm13.fnal.gov:/dune/app/users/weishi/FDCAFIntegration/srcs/duneana

# Build GEC code inside CAFMaker
cd duneana/duneana/CAFMaker
./build.sh

# Build/rebuild FD CAF maker code
cd /dune/app/users/weishi/FDCAFIntegration/srcs
mrb uc
cd ${MRB_BUILDDIR}       
mrb z
mrbsetenv
mrb b
```

Run CAF Maker interactively:
```
cd /dune/app/users/weishi/FDCAFIntegration/srcs/duneana/duneana/CAFMaker
lar -c select_ana_dune10kt_nu.fcl -n 10 /pnfs/dune/tape_backed/dunepro/mcc11/protodune/mc/full-reconstructed/07/51/31/11/nu_dune10kt_1x2x6_13009312_0_20181104T221530_gen_g4_detsim_reco.root
```

Relogin:
```
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup dunesw v09_68_00d00 -q e20:debug  
source /dune/app/users/weishi/FDCAFIntegration/localProducts_larsoft_v09_68_00_debug_e20/setup
mrbsetenv
```

Recompile:
```
cd ${MRB_BUILDDIR}                       
mrb z                               
mrbsetenv                            
mrb b
```

## Setup using a specific duneana version (v09_58_01d00)

```
mkdir GECFDCAF
cd GECFDCAF

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup dunesw v09_58_01d00 -q e20:debug

mrb newDev
source /dune/app/users/weishi/GECFDCAF/localProducts_larsoft_v09_58_01_debug_e20/setup

# Get FD maker code
cd /dune/app/users/weishi/GECFDCAF/srcs

# On local laptop, go to local copy and sync the changes in CAFMaker
cd ${LAPTOP_WORK_DIR}/duneana
rsync -e ssh -avSz  ./* weishi@dunegpvm13.fnal.gov:/dune/app/users/weishi/GECFDCAF/srcs/duneana

# After the local changes are pushed, replace the above with:
#   git clone https://github.com/weishi10141993/duneana.git -b v09_58_01d00  
# last update was Aug 19, 2022 on this tagged version, same on develop branch
# although this should be pushed to develop branch for future tagged version!

# Build GEC code inside CAFMaker
cd duneana/duneana/CAFMaker
./build.sh

# Build/rebuild FD CAF maker code
cd /dune/app/users/weishi/GECFDCAF/srcs
mrb uc
cd ${MRB_BUILDDIR}       
mrb z
mrbsetenv
mrb b
```

Run CAF Maker interactively:
```
cd /dune/app/users/weishi/GECFDCAF/srcs/duneana/duneana/CAFMaker
lar -c select_ana_dune10kt_nu.fcl -n 10 /pnfs/dune/tape_backed/dunepro/mcc11/protodune/mc/full-reconstructed/07/51/31/11/nu_dune10kt_1x2x6_13009312_0_20181104T221530_gen_g4_detsim_reco.root
```

Relogin:
```
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup dunesw v09_58_01d00 -q e20:debug
source /dune/app/users/weishi/GECFDCAF/localProducts_larsoft_v09_58_01_debug_e20/setup
mrbsetenv
```

Recompile:
```
cd ${MRB_BUILDDIR}                       
mrb z                               
mrbsetenv                            
mrb b
```

## Run TDR version FD CAFMaker (dune tpc on redmine)

Adapted from this [instruction](https://github.com/imawby/selection/wiki/Creating-'old-larsoft'-v07_09_00_e17_prof).

```
# Create a local larsoft work dir
mkdir TDRFDCAFMaker
cd TDRFDCAFMaker
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup larsoft v07_09_00 -q e17:prof
unsetup mrb
setup mrb v4_04_06
mrb newDev
source localProducts_larsoft_v07_09_00_e17_prof/setup

# Get dunetpc
cd srcs
mrb g dunetpc
cd dunetpc
git checkout tags/v07_09_00   # for PRISM purpose don't need to be this tag, but this tag works for sure

# replace the fcl with the following two
cd fcl/dunefd/mergeana
cp /dune/app/users/marshalc/mcc11_dunetpc/srcs/dunetpc/fcl/dunefd/mergeana/select_ana_dune10kt_nu.fcl .
# which uses this fcl file:
cp /dune/app/users/marshalc/mcc11_dunetpc/srcs/dunetpc/fcl/dunefd/mergeana/dunerw.fcl .

# Build the code
cd ../srcs
mrb uc
cd ${MRB_BUILDDIR}       
mrb z
mrbsetenv
unsetup cmake
setup cmake v3_19_6
mrb b

# Run the CAF maker production interactively
lar -c select_ana_dune10kt_nu.fcl -n 10 /pnfs/dune/tape_backed/dunepro/mcc11/protodune/mc/full-reconstructed/07/51/31/11/nu_dune10kt_1x2x6_13009312_0_20181104T221530_gen_g4_detsim_reco.root
```

Recompile if source code changed,

```
cd ${MRB_BUILDDIR}                   
mrb z
mrbsetenv  
make install -j 4  (how diff is this to mrb b)
```

Relogin,

```
cd /dune/app/users/weishi/FDCAFMaker
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup larsoft v07_09_00 -q e17:prof
unsetup mrb
setup mrb v4_04_06
source localProducts_larsoft_v07_09_00_e17_prof/setup
mrbsetenv
```
