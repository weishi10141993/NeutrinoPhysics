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

# To recompile from scratch (rarely used)
./standalone_configure_and_build.sh -u -r --db -f  

source build/Linux/CAFAnaEnv.sh
```

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
# update the right channame in it
root -l -b -q IntrinsicNue_StackedHist.C
```

Produce a fit,
```
# Edit what systematics to include
cd /dune/app/users/weishi/NueIntrinsic/lblpwgtools/CAFAna/PRISM/scripts/FermiGridPRISMScripts

./FarmCAFPRISMNodeScript.sh -c Dmsq32ScanCommands.cmd
FHC flagon: /pnfs/dune/persistent/users/weishi/IntrinsicNueFit/FHC/dmsq32_nodet/flagon
FHC flagoff: /pnfs/dune/persistent/users/weishi/IntrinsicNueFit/FHC/dmsq32_nodet/flagoff
RHC flagon: /pnfs/dune/persistent/users/weishi/IntrinsicNueFit/RHC/dmsq32_nodet/flagon
RHC flagoff: /pnfs/dune/persistent/users/weishi/IntrinsicNueFit/RHC/dmsq32_nodet/flagoff
4 Flavor flagon (double RAM 40G):  /pnfs/dune/persistent/users/weishi/IntrinsicNueFit/FourFlavor/dmsq32_nodet/flagon
4 Flavor flagoff (double RAM 40G): /pnfs/dune/persistent/users/weishi/IntrinsicNueFit/FourFlavor/dmsq32_nodet/flagoff

./FarmCAFPRISMNodeScript.sh -c Ssth23ScanCommands.cmd
FHC flagon:  /pnfs/dune/persistent/users/weishi/IntrinsicNueFit/FHC/ssth23_nodet/flagon
FHC flagoff: /pnfs/dune/persistent/users/weishi/IntrinsicNueFit/FHC/ssth23_nodet/flagoff
RHC flagon:  /pnfs/dune/persistent/users/weishi/IntrinsicNueFit/RHC/ssth23_nodet/flagon
RHC flagoff: /pnfs/dune/persistent/users/weishi/IntrinsicNueFit/RHC/ssth23_nodet/flagoff
4 Flavor flagon (double RAM 40G):  /pnfs/dune/persistent/users/weishi/IntrinsicNueFit/FourFlavor/ssth23_nodet/flagon
4 Flavor flagoff (double RAM 40G): /pnfs/dune/persistent/users/weishi/IntrinsicNueFit/FourFlavor/ssth23_nodet/flagoff

./FarmCAFPRISMNodeScript.sh -c DcpScanCommands.cmd
FHC flagon (old plot exists)
FHC flagoff (old plot exists)
RHC flagon  (old plot exists)
RHC flagoff (old plot exists)
4 Flavor flagon (double RAM 40G):  /pnfs/dune/persistent/users/weishi/IntrinsicNueFit/FourFlavor/dcp_nodet/flagon
4 Flavor flagoff (double RAM 40G): /pnfs/dune/persistent/users/weishi/IntrinsicNueFit/FourFlavor/dcp_nodet/flagoff

# Compare fit Chi2
cd /dune/app/users/weishi/NueIntrinsic/lblpwgtools/CAFAna/PRISM/scripts
# Edit pois, HornCurrent, Syst
root -l -b -q OverlaydChi2IntrinsicNueV1.C
```

## List of state files

## State file production with flux+xsec systs on FermiGrid

Below is an example of submit a state file production job to FermiGrid without detector systematic (i.e., xsec + flux systematics).

```
cd PRISM/scripts/FermiGridPRISMScripts/

# ND FHC only (564 files)
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/abooth/Production/ND_CAFMaker/nd_offaxis/v7/CAF/Hadded/subsets/FHC/ --no-fakedata-dials -a EVisReco --syst-descriptor "nodet" -N -u

# ND RHC only: on axis (350 files)
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/weishi/NDCAF/OnAxis --no-fakedata-dials -a EVisReco --syst-descriptor "nodet" -N -b

# ND RHC only: off axis (239 files)
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/abooth/Production/ND_CAFMaker/nd_offaxis/v7/CAF/Hadded/subsets/RHC_Attempt2 --no-fakedata-dials -a EVisReco --syst-descriptor "nodet" -N -b

# FD FHC only
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/chasnip/CAF_MC_FILES_4FLAVOUR/ --no-fakedata-dials -a EVisReco --syst-descriptor "nodet" -F -u  

# FD RHC only
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/chasnip/CAF_MC_FILES_4FLAVOUR/ --no-fakedata-dials -a EVisReco --syst-descriptor "nodet" -F -b
```

Hadd the output state files (can try SBU local cluster with more RAM):

```
ND FHC:
/pnfs/dune/persistent/users/weishi/CAFAnaInputs/StandardState/IntrinsicNue_Syst_nodet/NDFHC/batch1 (DONE)
/pnfs/dune/persistent/users/weishi/CAFAnaInputs/StandardState/IntrinsicNue_Syst_nodet/NDFHC/batch2 (DONE)
/pnfs/dune/persistent/users/weishi/CAFAnaInputs/StandardState/IntrinsicNue_Syst_nodet/NDFHC/batch3 (DONE)

ND RHC on axis:
/pnfs/dune/persistent/users/weishi/CAFAnaInputs/StandardState/IntrinsicNue_Syst_nodet/NDRHCOnAxis/batch1 (DONE)
/pnfs/dune/persistent/users/weishi/CAFAnaInputs/StandardState/IntrinsicNue_Syst_nodet/NDRHCOnAxis/batch2 (DONE)
/pnfs/dune/persistent/users/weishi/CAFAnaInputs/StandardState/IntrinsicNue_Syst_nodet/NDRHCOnAxis/batch3 (DONE)

ND RHC off axis: /pnfs/dune/persistent/users/weishi/CAFAnaInputs/StandardState/IntrinsicNue_Syst_nodet/NDRHCOffAxis (DONE)
FD FHC: /pnfs/dune/persistent/users/weishi/CAFAnaInputs/StandardState/IntrinsicNue_Syst_nodet/FDFHC
FD RHC: /pnfs/dune/persistent/users/weishi/CAFAnaInputs/StandardState/IntrinsicNue_Syst_nodet/FDRHC

./FarmHaddCafanaGrid.sh -i /pnfs/dune/persistent/users/weishi/CAFAnaInputs/StandardState/IntrinsicNue_Syst_nodet/Hadded (25-30G RAM is enough)
```

## Stat only state file production

Make a FHC state file:

```
# For ND
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/abooth/Production/ND_CAFMaker/nd_offaxis/v7/CAF/Hadded/subsets/FHC/ --no-fakedata-dials -a EVisReco --syst-descriptor "nosyst" -N -u

# For FD
MakePRISMPredInterps -o FDFHCState_IntrinsicNue_stat_only_withxseccorr.root -F-nu /dune/data/users/chasnip/OffAxisCAFs/FD_FHC_nonswap.root -Fe-nu /dune/data/users/chasnip/OffAxisCAFs/FD_FHC_nueswap.root -Ft-nu /dune/data/users/chasnip/OffAxisCAFs/FD_FHC_tauswap.root --bin-descriptor prism_default --no-fakedata-dials -A EVisReco --UseSelection --syst-descriptor "nosyst" > nue_intrinsic_state_file.txt &

# For debug purpose: use ETrue without selection
MakePRISMPredInterps -o FDFHCState_IntrinsicNue_xsec_49_to_54.root -F-nu /dune/data/users/chasnip/OffAxisCAFs/FD_FHC_nonswap.root -Fe-nu /dune/data/users/chasnip/OffAxisCAFs/FD_FHC_nueswap.root -Ft-nu /dune/data/users/chasnip/OffAxisCAFs/FD_FHC_tauswap.root --bin-descriptor default --no-fakedata-dials -A ETrue --syst-descriptor list:NR_nubar_n_NC_1Pi:NR_nubar_n_NC_2Pi:NR_nubar_n_NC_3Pi:NR_nubar_p_NC_1Pi:NR_nubar_p_NC_2Pi:NR_nubar_p_NC_3Pi > nue_intrinsic_state_file.txt &

# Add ND and FD files
cd /dune/app/users/weishi/NueIntrinsic/lblpwgtools/CAFAna/bin
hadd_cafana NDFHCState_IntrinsicNue_stat_only_withxseccorr.root /pnfs/dune/persistent/users/weishi/CAFAnaInputs/StandardState/FHCIntrinsicNue/*.root
hadd_cafana hadd_FHC_state_IntrinsicNue_stat_only_withxseccorr.root NDFHCState_IntrinsicNue_stat_only_withxseccorr.root FDFHCState_IntrinsicNue_stat_only_withxseccorr.root
```

Make a RHC state file:

```
# For RHC ND, need to run twice with on and off axis, then hadd_cafana
# RHC on axis: /pnfs/dune/persistent/users/abooth/Production/ND_CAFMaker/nd_offaxis/v7/CAF/Hadded/subsets/RHC/*_0m*.root
# RHC off axis: /pnfs/dune/persistent/users/abooth/Production/ND_CAFMaker/nd_offaxis/v7/CAF/Hadded/subsets/RHC_Attempt2

#
# Stat only
#

# ND ON AXIS
# copied from: /pnfs/dune/persistent/users/abooth/Production/ND_CAFMaker/nd_offaxis/v7/CAF/Hadded/subsets/RHC/*_0m*.root
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/weishi/NDCAF/OnAxis --no-fakedata-dials -a EVisReco --syst-descriptor "nosyst" -N -b

# ND Off AXIS
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/abooth/Production/ND_CAFMaker/nd_offaxis/v7/CAF/Hadded/subsets/RHC_Attempt2 --no-fakedata-dials -a EVisReco --syst-descriptor "nosyst" -N -b

# FD  
MakePRISMPredInterps -o FDRHCState_IntrinsicNue_stat_only_withxseccorr.root -F-nub /dune/data/users/chasnip/OffAxisCAFs/FD_RHC_nonswap.root -Fe-nub /dune/data/users/chasnip/OffAxisCAFs/FD_RHC_nueswap.root -Ft-nub /dune/data/users/chasnip/OffAxisCAFs/FD_RHC_tauswap.root --bin-descriptor prism_default --no-fakedata-dials -A EVisReco --UseSelection --syst-descriptor "nosyst" > nue_intrinsic_state_file.txt &

# add files
hadd_cafana NDOnAxisRHCState_IntrinsicNue_stat_only_withxseccorr.root /pnfs/dune/persistent/users/weishi/CAFAnaInputs/StandardState/RHCIntrinsicNue/NDOnAxis/*.root
hadd_cafana NDOffAxisRHCState_IntrinsicNue_stat_only_withxseccorr.root /pnfs/dune/persistent/users/weishi/CAFAnaInputs/StandardState/RHCIntrinsicNue/NDOffAxis/*.root
hadd_cafana hadd_RHC_state_IntrinsicNue_stat_only_withxseccorr.root NDOnAxisRHCState_IntrinsicNue_stat_only_withxseccorr.root NDOffAxisRHCState_IntrinsicNue_stat_only_withxseccorr.root FDRHCState_IntrinsicNue_stat_only_withxseccorr.root
```

Verify FHC state file contains non-empty plot in ```FDMatchInterp_ETrue_numu_nu/pred_nom/extrap/nue_surv``` folder.
Verify RHC state file contains non-empty plot in ```FDMatchInterp_ETrue_numu_nub/pred_nom/extrap/nue_surv_anti``` folder.
