# Run FD CAFMaker dunetpc legacy code (TDR version FD CAF)

## Setup

```
mkdir dunetpclegacy
cd dunetpclegacy

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh

setup larsoft v07_09_00 -q e17:prof
unsetup mrb
setup mrb v4_04_06
mrb newDev
source /dune/app/users/weishi/dunetpclegacy/localProducts_larsoft_v07_09_00_e17_prof/setup

cd srcs

# On local laptop:
#   git clone https://github.com/weishi10141993/dunetpc.git -b feature/v07_09_00
# sync the changes in CAFMaker
cd ${LAPTOP_WORK_DIR}/dunetpc
rsync -e ssh -avSz  ./* weishi@dunegpvm13.fnal.gov:/dune/app/users/weishi/dunetpclegacy/srcs/dunetpc
# Should remove pandora reco and use legacy CAF reco

mrb g nusystematics
cd nusystematics
git checkout remotes/origin/TDRSensProd
# Change this line in ups/product_deps: nutools v2_24_07 to nutools v2_24_05

mrb g systematicstools
cd systematicstools
git checkout remotes/origin/TDRSensProd

/////
mrb g larcoreobj
cd larcoreobj
git checkout tags/LARSOFT_SUITE_v07_09_00

mrb g lardataobj
cd lardataobj
git checkout tags/LARSOFT_SUITE_v07_09_00

mrb g larpandora
cd larpandora
git checkout tags/LARSOFT_SUITE_v07_09_00

mrb g larsim
cd larsim
git checkout tags/LARSOFT_SUITE_v07_09_00
# comment out find_ups_product( ifdhc ) in CMakeLists.txt

mrb g larreco
cd larreco
git checkout tags/LARSOFT_SUITE_v07_09_00

mrb g nusimdata
cd nusimdata
git checkout tags/v1_14_01
/////


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
lar -c ../../fcl/dunefd/mergeana/select_ana_dune10kt_nu.fcl -n 100 /pnfs/dune/tape_backed/dunepro/mcc11/protodune/mc/full-reconstructed/07/51/31/11/nu_dune10kt_1x2x6_13009312_0_20181104T221530_gen_g4_detsim_reco.root
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
