# Instruction for DUNE-PRISM analysis setup from DUNE FNAL machines (dunegpvm*)

## NuTau Appearance Development [work on nnhome machine]

[First time only]

```
ssh -X <user_name>@nnhome.physics.sunysb.edu
# Request an interactive node for compiling !!! NEVER COMPILE ON nnhome !!!
srun --pty bash -i

mkdir nutau
cd /home/wshi/nutau

# Go to your local laptop
git clone https://github.com/weishi10141993/lblpwgtools.git -b nuTau_dev     # the idea is to merge this branch to the YOLO branch
cd lblpwgtools

# Sync local code changes to remote nnhome machine
rsync -e ssh -avSz  ./* <user_name>@nnhome.physics.sunysb.edu:/home/<user_name>/nutau/lblpwgtools

# Go back to nnhome
cd /home/wshi/nutau/lblpwgtools/CAFAna

# The build below requires the following softwares installed on nnhome
# Make sure you include these lines in ~/.profile:
################################################
# set ROOT PATH
#if [ -d "$HOME/ROOT/root_install/bin" ]; then
#    PATH="$HOME/ROOT/root_install/bin:$PATH"
#fi
# OR these
ROOTSYS="/home/wshi/ROOT/root_install"
PATH=$PATH:$ROOTSYS/bin
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ROOTSYS/lib
# OR change order
#ROOTSYS="/home/wshi/ROOT/root_install"
#PATH=$ROOTSYS/bin:$PATH
#LD_LIBRARY_PATH=$ROOTSYS/lib
# OR
#source /home/wshi/ROOT/root_install/bin/thisroot.sh

# Homebrew
eval "$(/home/wshi/.linuxbrew/bin/brew shellenv)"
################################################

################################################
# Before compile:
#   1) NEED to add: -Wl,-rpath,${ROOT_LIB}
#      to LDFLAGS_BINS in OscLib/Makefile:L16
# This is already done in weishi10141993/OscLib.git
################################################

# Build the code    
# Do not use '-u' option as it relies on cvmfs which is not mounted on nnhome.
./standalone_configure_and_build.sh -r --db

# set CAFAna environment
source build/Linux/CAFAnaEnv.sh

# To recompile code changes
cd build
make install -j 4

# To recompile from scratch (rarely used)
./standalone_configure_and_build.sh -r --db -f
```

## File locations

The input CAF files will be copied to ```/storage/shared```. All large file output should be stored in the same area instead of home area!

### Install ROOT6 on nnhome

ROOT6 is required for the code to compile. ROOT6 has been installed at my home area. The following instruction is for future reference only.

Instruction is adapted from: https://root.cern/install/build_from_source/

```
[First time only]

cd /home/wshi/
mkdir ROOT
cd ROOT

git clone https://github.com/root-project/root.git
cd root
git checkout -b v6-22-08 v6-22-08
cd ..

mkdir root_build root_install && cd root_build

# Build ROOT with c++17 as later OscLib uses c++17
# ROOT needs to be configured and built with the same C++ standard
# as the programs that will make use of it.
# We also need minuit2
cmake -DCMAKE_CXX_STANDARD=17 -Dminuit2=ON -DCMAKE_INSTALL_PREFIX=../root_install ../root
# Default: cmake -DCMAKE_INSTALL_PREFIX=../root_install ../root    # CMake will check your environment before build

# Speedup the build with:
cmake --build . -- install -j4
# 4 is the number of available cores. Use 'lscpu' to find out how many cores and check entry 'Core(s) per socket'
# there are some warnings of deprecated functions in python3.9: /usr/include/python3.9, but the build is successful
# original command: cmake --build . --target install                        # Build

# v6-22-08
source /home/wshi/ROOT/root_install/bin/thisroot.sh

# Check ROOT C++ compiler:
root-config --features
# This is the output:
#   cxx17 asimage builtin_afterimage builtin_clang builtin_ftgl builtin_glew builtin_llvm builtin_lz4 builtin_tbb builtin_vdt builtin_xrootd builtin_xxhash builtin_zstd clad dataframe davix exceptions fftw3 gdml http imt mathmore mlp minuit2 opengl pyroot roofit webgui root7 runtime_cxxmodules shared ssl tmva tmva-cpu tmva-pymva spectrum vdt x11 xml xrootd
# So ROOT is compiled for C++17

# nnhome has C++ compiler version: g++ (Debian 10.2.1-6) 10.2.1 20210110

# if want to remove root
rm -rf /home/wshi/ROOT/root_install
```

### Install Homebrew on nnhome

[Homebrew](https://brew.sh/) is required for the code to compile. It has been installed at my home area. The following instruction is for future reference only.

```
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
# After done, add Homebrew to your ~/.bashrc or ~/.profile:
eval "$(/home/wshi/.linuxbrew/bin/brew shellenv)"
```

It installs at ```/home/wshi/.linuxbrew```.

### Install gsl-config on nnhome

This turns out not necessary anymore as trees on nnhome has ```gsl``` installed already.

The following is from this [tutorial](https://coral.ise.lehigh.edu/jild13/2016/07/11/hello/).

```
wget ftp://ftp.gnu.org/gnu/gsl/gsl-2.7.tar.gz
tar -zxvf gsl-2.7.tar.gz
cd gsl-2.7

# create install dir
mkdir /home/wshi/gsl
./configure --prefix=/home/wshi/gsl
# If there are no errors, compile the library.
make
# check and test the library before actually installing it.
make check
# install the library
make install

# source it in ~/.bashrc
source /home/wshi/gsl/bin/gsl-config
# Or try:
export GSL_CONFIG=/home/wshi/gsl/bin/gsl-config
export PATH=/home/wshi/gsl/bin:$PATH
# the above will probably lock you outside the nnhome
# do this to avoid running .bashrc or .profile:
# ssh -t user@host bash --noprofile --norc
```

## NuTau Appearance Development [work on FNAL machine]

[First time only]

```
kinit -f weishi@FNAL.GOV                 # use your FNAL kerberos password
ssh -X weishi@dunegpvm03.fnal.gov        
cd /dune/app/users/weishi
mkdir NuTauDev
cd NuTauDev
git clone https://github.com/weishi10141993/lblpwgtools.git
cd lblpwgtools
git checkout nuTau_dev                   # branch is based on Catastrophic_Yolo_PRISM_Merge
cd CAFAna
# Build the code, the -u option rely on relevant dependencies from FNAL scisoft
./standalone_configure_and_build.sh -u -r --db          # for new YOLO branch
# To recompile: ./standalone_configure_and_build.sh -u -r --db -f
source build/Linux/CAFAnaEnv.sh
```

The changed places are listed here:

```
Added/changed var                               File location

# Nutau selection
kPassFD_CVN_NUTAU                               Cuts/AnaCuts.h
# This can be changed in the future to other nutau selections: e.g. RecoHadE_NDFD (Core/SpectrumLoader.cxx) or kHadEReco (Analysis/AnalysisVars.cxx)

# Signal state file
kPRISMFDSignal_True(Selected)_nutau(b)          PRISM/Cuts.h(.cxx)
kFDSelectionCuts_nutau(b)                       PRISM/app/MakePRISMPredInterps.C
FDCuts                                          PRISM/app/MakePRISMPredInterps.C
kNutau/kNutauBar/kNutauNutauBar                 PRISM/PRISMAnalysisDefinitions.h
chanmode                                        PRISM/app/MakePRISMPredInterps.C
FarDetData_nutauswap                            PRISM/app/MakePRISMPredInterps.C
Nutau_app/Nutaubar_app                          fcl/PRISM/FitChannels.fcl
FDSigFlavor/Sign/FDWrong/IntrinsicFlavor        PRISM/PredictionPRISM.cxx
GetFDPrediction_right_sign_nue                  PRISM/PredictionPRISM.cxx
GetFDUnOscWeightedSigPrediction_right_sign_nue  PRISM/PredictionPRISM.cxx
FDNutauSwapAppOscPrediction                     PRISM/PredictionPRISM.cxx
LoadPRISMState                                  PRISM/PRISMUtils.cxx
fSpectrumNutauSwap                              Prediction/PredictionsForPRISM
```

Make a FHC state file:
```
# run ND only
# change AXISBLOBNAME to uniform_coarse for finer bins
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/abooth/Production/ND_CAFMaker/nd_offaxis/v7/CAF/Hadded/subsets/FHC/ --no-fakedata-dials -a EVisReco --syst-descriptor list:NR_nubar_n_NC_1Pi:NR_nubar_n_NC_2Pi:NR_nubar_n_NC_3Pi:NR_nubar_p_NC_1Pi:NR_nubar_p_NC_2Pi:NR_nubar_p_NC_3Pi -N -u  

# run FD only
# For finer bins: --bin-descriptor uniform_coarse
MakePRISMPredInterps -o FDFHCStateNuTau_xsec_49_to_54_uniform_coarse_binning.root -F-nu /dune/data/users/chasnip/OffAxisCAFs/FD_FHC_nonswap.root -Fe-nu /dune/data/users/chasnip/OffAxisCAFs/FD_FHC_nueswap.root -Ft-nu /dune/data/users/chasnip/OffAxisCAFs/FD_FHC_tauswap.root --bin-descriptor uniform_coarse --no-fakedata-dials -A EVisReco --UseSelection --syst-descriptor list:NR_nubar_n_NC_1Pi:NR_nubar_n_NC_2Pi:NR_nubar_n_NC_3Pi:NR_nubar_p_NC_1Pi:NR_nubar_p_NC_2Pi:NR_nubar_p_NC_3Pi > nutau_state_file.txt &

# add ND and FD files
cd /dune/app/users/weishi/NuTauDev/lblpwgtools/CAFAna/bin
hadd_cafana NDFHCStateNuTau_xsec_49_to_54_uniform_coarse_binning.root /pnfs/dune/persistent/users/weishi/CAFAnaInputs/StandardState/*.root
hadd_cafana hadd_state_file_xsec_49_to_54_uniform_coarse_binning.root NDFHCStateNuTau_xsec_49_to_54_uniform_coarse_binning.root FDFHCStateNuTau_xsec_49_to_54_uniform_coarse_binning.root

# FHC Only (ND+FD together, interactively, longer time)
MakePRISMPredInterps -o hadd_state_file_xsec_49_to_54.root -N-nu "/pnfs/dune/persistent/users/abooth/Production/ND_CAFMaker/nd_offaxis/v7/CAF/Hadded/subsets/FHC/*.root" -F-nu /dune/data/users/chasnip/OffAxisCAFs/FD_FHC_nonswap.root -Fe-nu /dune/data/users/chasnip/OffAxisCAFs/FD_FHC_nueswap.root -Ft-nu /dune/data/users/chasnip/OffAxisCAFs/FD_FHC_tauswap.root --bin-descriptor default --no-fakedata-dials -A EVisReco --UseSelection --syst-descriptor list:NR_nubar_n_NC_1Pi:NR_nubar_n_NC_2Pi:NR_nubar_n_NC_3Pi:NR_nubar_p_NC_1Pi:NR_nubar_p_NC_2Pi:NR_nubar_p_NC_3Pi > nutau_state_file.txt &
```

Make a RHC state file:
```
# with all
# for RHC ND, need to run twice with on and off axis, then hadd_cafana
# RHC on axis: /pnfs/dune/persistent/users/abooth/Production/ND_CAFMaker/nd_offaxis/v7/CAF/Hadded/subsets/RHC/*_0m*.root
# RHC off axis: /pnfs/dune/persistent/users/abooth/Production/ND_CAFMaker/nd_offaxis/v7/CAF/Hadded/subsets/RHC_Attempt2
MakePRISMPredInterps -o hadd_state_file_xsec_49_to_54.root -N-nu "/pnfs/dune/persistent/users/abooth/Production/ND_CAFMaker/nd_offaxis/v7/CAF/Hadded/subsets/FHC/*.root" -F-nu /dune/data/users/chasnip/OffAxisCAFs/FD_FHC_nonswap.root -Fe-nu /dune/data/users/chasnip/OffAxisCAFs/FD_FHC_nueswap.root -Ft-nu /dune/data/users/chasnip/OffAxisCAFs/FD_FHC_tauswap.root -N-nub "/pnfs/dune/persistent/users/abooth/Production/ND_CAFMaker/nd_offaxis/v7/CAF/Hadded/subsets/RHC/*.root" -F-nub /dune/data/users/chasnip/OffAxisCAFs/FD_RHC_nonswap.root -Fe-nub /dune/data/users/chasnip/OffAxisCAFs/FD_RHC_nueswap.root -Ft-nub /dune/data/users/chasnip/OffAxisCAFs/FD_RHC_tauswap.root --bin-descriptor default --no-fakedata-dials -A EVisReco --UseSelection --syst-descriptor list:NR_nubar_n_NC_1Pi:NR_nubar_n_NC_2Pi:NR_nubar_n_NC_3Pi:NR_nubar_p_NC_1Pi:NR_nubar_p_NC_2Pi:NR_nubar_p_NC_3Pi > nutau_state_file.txt &
```

Produce plots,

```
cd /dune/app/users/weishi/NuTauDev/lblpwgtools/CAFAna/PRISM/app
PRISMPrediction ../../fcl/PRISM/NuisanceSyst_Scan/Basic_PRISMPred_PlaceHolder.fcl
```

Plot stacked histogram,

```
cd /dune/app/users/weishi/NuTauDev/lblpwgtools/CAFAna/PRISM/scripts
# Get the plotting script [One time only]
wget https://raw.githubusercontent.com/weishi10141993/NeutrinoPhysics/main/NutauApp_StackedHist.C
root -l -b -q NutauApp_StackedHist.C
```

Produce a fit,
```
cd /dune/app/users/weishi/NuTauDev/lblpwgtools/CAFAna/PRISM/scripts/FermiGridPRISMScripts
./FarmCAFPRISMNodeScript.sh -c Dmsq32ScanCommands.cmd # this includes both dmsq32 and ssth23
```
