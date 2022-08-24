# Instruction for running FD CAF Maker from DUNE FNAL machines (dunegpvm*)

## Run most recent FD CAFMaker (duneana in DUNE git)

```
mkdir NewFDCAFMaker
cd NewFDCAFMaker

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup larsoft v09_56_00 -q e20:debug

mrb newDev
source /dune/app/users/weishi/NewFDCAFMaker/localProducts_larsoft_v09_56_00_debug_e20/setup

# Get duneana
cd srcs
mrb g duneana (develop default)
mrb g dunesim (develop default)
mrb g dunecore

# Build the code
mrb uc
cd ${MRB_BUILDDIR}       
mrb z
mrbsetenv
mrb b
```

Run CAF Maker interactively:
```
cd /dune/app/users/weishi/NewFDCAFMaker/srcs/duneana/duneana/CAFMaker
lar -c cafmakerjob.fcl -n 10 /pnfs/dune/tape_backed/dunepro/mcc11/protodune/mc/full-reconstructed/07/51/31/11/nu_dune10kt_1x2x6_13009312_0_20181104T221530_gen_g4_detsim_reco.root
```

Recompile if source code changed,

```
cd ${MRB_BUILDDIR}                   
mrb z
mrbsetenv  
make install -j 4  (how diff is this to mrb b)
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
