# Instruction for DUNE-PRISM analysis setup from DUNE FNAL machines (dunegpvm*)

## Install CAFAna Framework

[First time only]

```
kinit -f weishi@FNAL.GOV                 # use your FNAL kerberos password
ssh -X weishi@dunegpvm03.fnal.gov        
cd /dune/app/users/weishi
mkdir NueIntrinsic
cd NueIntrinsic
git clone https://github.com/weishi10141993/lblpwgtools.git -b Catastrophic_Yolo_PRISM_Merge
cd lblpwgtools/CAFAna
# Build the code, the -u option rely on relevant dependencies from FNAL scisoft
./standalone_configure_and_build.sh -u -r --db

# To recompile code changes
cd build
make install -j 4

# To recompile from scratch (rarely used):
./standalone_configure_and_build.sh -u -r --db -f  

source build/Linux/CAFAnaEnv.sh
```

## Code changes

Make changes:

```
MakePRISMPredInterps.C: add kIsBeamNue to MatchPredGens
PredictionsForPRISM.cxx: add intrinsic nue to PredictComponent
PRISMExtrapolator.cxx: add nue spectrum to target flux
```

Produce a FHC state file,

```
# run ND only
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/abooth/Production/ND_CAFMaker/nd_offaxis/v7/CAF/Hadded/subsets/FHC/ --no-fakedata-dials -a EVisReco --syst-descriptor list:NR_nubar_n_NC_1Pi:NR_nubar_n_NC_2Pi:NR_nubar_n_NC_3Pi:NR_nubar_p_NC_1Pi:NR_nubar_p_NC_2Pi:NR_nubar_p_NC_3Pi -N -u

# run FD only
MakePRISMPredInterps -o FDFHCState_IntrinsicNue_xsec_49_to_54.root -F-nu /dune/data/users/chasnip/OffAxisCAFs/FD_FHC_nonswap.root -Fe-nu /dune/data/users/chasnip/OffAxisCAFs/FD_FHC_nueswap.root -Ft-nu /dune/data/users/chasnip/OffAxisCAFs/FD_FHC_tauswap.root --bin-descriptor prism_default --no-fakedata-dials -A EVisReco --UseSelection --syst-descriptor list:NR_nubar_n_NC_1Pi:NR_nubar_n_NC_2Pi:NR_nubar_n_NC_3Pi:NR_nubar_p_NC_1Pi:NR_nubar_p_NC_2Pi:NR_nubar_p_NC_3Pi > nue_intrinsic_state_file.txt &

# for debug purpose: use ETrue without selection
MakePRISMPredInterps -o FDFHCState_IntrinsicNue_xsec_49_to_54.root -F-nu /dune/data/users/chasnip/OffAxisCAFs/FD_FHC_nonswap.root -Fe-nu /dune/data/users/chasnip/OffAxisCAFs/FD_FHC_nueswap.root -Ft-nu /dune/data/users/chasnip/OffAxisCAFs/FD_FHC_tauswap.root --bin-descriptor default --no-fakedata-dials -A ETrue --syst-descriptor list:NR_nubar_n_NC_1Pi:NR_nubar_n_NC_2Pi:NR_nubar_n_NC_3Pi:NR_nubar_p_NC_1Pi:NR_nubar_p_NC_2Pi:NR_nubar_p_NC_3Pi > nue_intrinsic_state_file.txt &

# add ND and FD files
cd /dune/app/users/weishi/NueIntrinsic/lblpwgtools/CAFAna/bin
hadd_cafana NDFHCState_IntrinsicNue_xsec_49_to_54.root /pnfs/dune/persistent/users/weishi/CAFAnaInputs/StandardState/FHCIntrinsicNue/*.root
hadd_cafana hadd_FHC_state_IntrinsicNue_xsec_49_to_54.root NDFHCState_IntrinsicNue_xsec_49_to_54.root FDFHCState_IntrinsicNue_xsec_49_to_54.root
```

Make a RHC state file:

```
# for RHC ND, need to run twice with on and off axis, then hadd_cafana
# RHC on axis: /pnfs/dune/persistent/users/abooth/Production/ND_CAFMaker/nd_offaxis/v7/CAF/Hadded/subsets/RHC/*_0m*.root
# RHC off axis: /pnfs/dune/persistent/users/abooth/Production/ND_CAFMaker/nd_offaxis/v7/CAF/Hadded/subsets/RHC_Attempt2

# run ND on-axis only
# copied from: /pnfs/dune/persistent/users/abooth/Production/ND_CAFMaker/nd_offaxis/v7/CAF/Hadded/subsets/RHC/*_0m*.root
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/weishi/NDCAF/OnAxis --no-fakedata-dials -a EVisReco --syst-descriptor list:NR_nubar_n_NC_1Pi:NR_nubar_n_NC_2Pi:NR_nubar_n_NC_3Pi:NR_nubar_p_NC_1Pi:NR_nubar_p_NC_2Pi:NR_nubar_p_NC_3Pi -N -b

# run ND off-axis only
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/abooth/Production/ND_CAFMaker/nd_offaxis/v7/CAF/Hadded/subsets/RHC_Attempt2 --no-fakedata-dials -a EVisReco --syst-descriptor list:NR_nubar_n_NC_1Pi:NR_nubar_n_NC_2Pi:NR_nubar_n_NC_3Pi:NR_nubar_p_NC_1Pi:NR_nubar_p_NC_2Pi:NR_nubar_p_NC_3Pi -N -b

# run FD only
MakePRISMPredInterps -o FDRHCState_IntrinsicNue_xsec_49_to_54.root -F-nub /dune/data/users/chasnip/OffAxisCAFs/FD_RHC_nonswap.root -Fe-nub /dune/data/users/chasnip/OffAxisCAFs/FD_RHC_nueswap.root -Ft-nub /dune/data/users/chasnip/OffAxisCAFs/FD_RHC_tauswap.root --bin-descriptor prism_default --no-fakedata-dials -A EVisReco --UseSelection --syst-descriptor list:NR_nubar_n_NC_1Pi:NR_nubar_n_NC_2Pi:NR_nubar_n_NC_3Pi:NR_nubar_p_NC_1Pi:NR_nubar_p_NC_2Pi:NR_nubar_p_NC_3Pi > nue_intrinsic_state_file.txt &

# add files
hadd_cafana NDOnAxisRHCState_IntrinsicNue_xsec_49_to_54.root /pnfs/dune/persistent/users/weishi/CAFAnaInputs/StandardState/RHCIntrinsicNue/NDOnAxis/*.root
hadd_cafana NDOffAxisRHCState_IntrinsicNue_xsec_49_to_54.root /pnfs/dune/persistent/users/weishi/CAFAnaInputs/StandardState/RHCIntrinsicNue/NDOffAxis/*.root
hadd_cafana hadd_rhc_state_intrinsic_nue_file_xsec_49_to_54.root NDOnAxisRHCState_IntrinsicNue_xsec_49_to_54.root NDOffAxisRHCState_IntrinsicNue_xsec_49_to_54.root FDRHCState_IntrinsicNue_xsec_49_to_54.root
```

Verify FHC state file contain non-empty plot in ```FDMatchInterp_ETrue_numu_nu/pred_nom/extrap/nue_surv``` folder.
Verify RHC state file contain non-empty plot in ```FDMatchInterp_ETrue_numu_nub/pred_nom/extrap/nue_surv_anti``` folder.

Produce plots,

```
cd /dune/app/users/weishi/NueIntrinsic/lblpwgtools/CAFAna/PRISM/app
# Change match_intrinsic_nue to true
PRISMPrediction ../../fcl/PRISM/NuisanceSyst_Scan/Basic_PRISMPred_PlaceHolder.fcl
```

Plot stacked histogram,

```
cd /dune/app/users/weishi/NueIntrinsic/lblpwgtools/CAFAna/PRISM/app
# Get the plotting script [One time only]
wget https://raw.githubusercontent.com/weishi10141993/NeutrinoPhysics/main/IntrinsicNue_StackedHist.C
# Do not include intrinsic nue MC in stacked histogram
root -l -b -q IntrinsicNue_StackedHist.C
```
