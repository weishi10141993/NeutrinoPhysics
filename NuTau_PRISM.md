# Instruction for DUNE-PRISM analysis setup from DUNE FNAL machines (dunegpvm*)

## NuTau Appearance Development [work on nnhome machine]

[First time only]

```
ssh -X wshi@nnhome.physics.sunysb.edu
mkdir nutau
cd /home/wshi/nutau

# check nutau code is committed
# the idea is to commit to the main YOLO branch and delete the nuTau_dev branch

git clone https://github.com/weishi10141993/lblpwgtools.git -b Catastrophic_Yolo_PRISM_Merge

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
