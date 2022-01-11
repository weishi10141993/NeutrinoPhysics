# Instruction for DUNE-PRISM analysis setup from DUNE FNAL machines (dunegpvm*)


## Install CAFAna Framework

[First time only]

```
kinit -f weishi@FNAL.GOV                 # use your FNAL kerberos password
ssh -X weishi@dunegpvm03.fnal.gov        
cd /dune/app/users/weishi
mkdir PRISMAnalysis
cd PRISMAnalysis
git clone https://github.com/weishi10141993/lblpwgtools.git
cd lblpwgtools
git checkout feature/PRISM
cd CAFAna
# Build the code, the -u option rely on relevant dependencies from FNAL scisoft
./standalone_configure_and_build.sh --use-PRISM -u --rdb
# To recompile: ./standalone_configure_and_build.sh --use-PRISM -u --rdb -f
# May need to set export GSL_LIB=/usr/lib64
```

### Study systematic impacts on DUNE PRISM event rates

1. First produce the state files with systematics.

```
cd /dune/app/users/weishi/PRISMAnalysis/lblpwgtools/CAFAna
source build/Linux/CAFAnaEnv.sh                                 # set up the environment
# Ignore the error of can't find the example directory
```

The full list of systematics are,

```
# Xsec systs (56 in tot)
list:MaCCQE:VecFFCCQEshape:CCQEPauliSupViaKF:MaCCRES:MvCCRES:Theta_Delta2Npi:AhtBY:BhtBY:CV1uBY:CV2uBY:MaNCRES:MvNCRES:FrCEx_N:FrElas_N:FrInel_N:FrAbs_N:FrPiProd_N:FrCEx_pi:FrElas_pi:FrInel_pi:FrAbs_pi:FrPiProd_pi:BeRPA_A:BeRPA_B:BeRPA_D:C12ToAr40_2p2hScaling_nu:C12ToAr40_2p2hScaling_nubar:E2p2h_A_nu:E2p2h_B_nu:E2p2h_A_nubar:E2p2h_B_nubar:NR_nu_np_CC_1Pi:NR_nu_n_CC_2Pi:NR_nu_n_CC_3Pi:NR_nu_p_CC_2Pi:NR_nu_p_CC_3Pi:NR_nu_n_NC_1Pi:NR_nu_n_NC_2Pi:NR_nu_n_NC_3Pi:NR_nu_p_NC_1Pi:NR_nu_p_NC_2Pi:NR_nu_p_NC_3Pi:NR_nubar_n_CC_1Pi:NR_nubar_n_CC_2Pi:NR_nubar_n_CC_3Pi:NR_nubar_p_CC_1Pi:NR_nubar_p_CC_2Pi:NR_nubar_p_CC_3Pi:NR_nubar_n_NC_1Pi:NR_nubar_n_NC_2Pi:NR_nubar_n_NC_3Pi:NR_nubar_p_NC_1Pi:NR_nubar_p_NC_2Pi:NR_nubar_p_NC_3Pi:nuenumu_xsec_ratio:nuenuebar_xsec_ratio

# Flux systs (28 in tot, there are more flux PCA systs, but now just use first 15 PCA components: 13-27)
list:flux_Nov17_0:flux_Nov17_1:flux_Nov17_2:flux_Nov17_3:flux_Nov17_4:flux_Nov17_5:flux_Nov17_6:flux_Nov17_7:flux_Nov17_8:flux_Nov17_9:flux_Nov17_10:flux_Nov17_11:flux_Nov17_12:flux_Nov17_13:flux_Nov17_14:flux_Nov17_15:flux_Nov17_16:flux_Nov17_17:flux_Nov17_18:flux_Nov17_19:flux_Nov17_20:flux_Nov17_21:flux_Nov17_22:flux_Nov17_23:flux_Nov17_24:flux_Nov17_25:flux_Nov17_26:flux_Nov17_27

# Detector systs (39 in tot)
(!!! These 6 are not working for now !!!) list:FDRecoNumuSyst:FDRecoNueSyst:FVNumuFD:FVNueFD:RecoNCSyst:FVNumuND
list:RecoEnergyScaleND:RecoEnergySqrtND:RecoEnergyInvSqrtND:EMRecoUncorrND:EMRecoUncorrSqrtND:EMRecoUncorrInvSqrtND:ChargedHadRecoUncorrND:ChargedHadRecoUncorrSqrtND:ChargedHadRecoUncorrInvSqrtND:ERecoScaleMuLArND:ERecoScaleMuLArSqrtND:ERecoScaleMuLArInvSqrtND:MuonRecoResND:EMRecoResND:ChargedHadRecoResND
list:RecoEnergyScaleFD:RecoEnergySqrtFD:RecoEnergyInvSqrtFD:EMRecoUncorrFD:EMRecoUncorrSqrtFD:EMRecoUncorrInvSqrtFD:ChargedHadRecoUncorrFD:ChargedHadRecoUncorrSqrtFD:ChargedHadRecoUncorrInvSqrtFD:ERecoScaleMuLArFD:ERecoScaleMuLArSqrtFD:ERecoScaleMuLArInvSqrtFD:MuonRecoResFD:EMRecoResFD:ChargedHadRecoResFD
list:NuOnECCBkgSyst:NuOnENCBkgSyst:NuOnERecoEffSyst
```

To run interactively with flux + cross section systematics,

```
# 1-year ND MC
FHC: /pnfs/dune/persistent/users/chasnip/CAF_MC_FILES_4FLAVOUR/ND_FHC_v7Full_6E20OnAxis_absxPOT_Apr21.root
RHC: /dune/data/users/chasnip/CombinedNDCAFs/ND_RHC_absxPOT_Sep21.root

# 3.5-year ND MC
FHC: /pnfs/dune/persistent/users/abooth/Production/ND_CAFMaker/nd_offaxis/v7/CAF/Hadded/subsets/FHC/*.root
RHC: /pnfs/dune/persistent/users/abooth/Production/ND_CAFMaker/nd_offaxis/v7/CAF/Hadded/subsets/RHC/*.root

cd PRISM/app
# For usage: MakePRISMPredInterps --help

# Run w/ 3.5 yr ND MC
MakePRISMPredInterps -o hadd_state_file_xsec_49_to_54.root -N-nu "/pnfs/dune/persistent/users/abooth/Production/ND_CAFMaker/nd_offaxis/v7/CAF/Hadded/subsets/FHC/*.root" -F-nu /dune/data/users/chasnip/OffAxisCAFs/FD_FHC_nonswap.root -Fe-nu /dune/data/users/chasnip/OffAxisCAFs/FD_FHC_nueswap.root -Ft-nu /dune/data/users/chasnip/OffAxisCAFs/FD_FHC_tauswap.root -N-nub "/pnfs/dune/persistent/users/abooth/Production/ND_CAFMaker/nd_offaxis/v7/CAF/Hadded/subsets/RHC/*.root" -F-nub /dune/data/users/chasnip/OffAxisCAFs/FD_RHC_nonswap.root -Fe-nub /dune/data/users/chasnip/OffAxisCAFs/FD_RHC_nueswap.root -Ft-nub /dune/data/users/chasnip/OffAxisCAFs/FD_RHC_tauswap.root --bin-descriptor default --no-fakedata-dials -A EVisReco --UseSelection --syst-descriptor list:NR_nubar_n_NC_1Pi:NR_nubar_n_NC_2Pi:NR_nubar_n_NC_3Pi:NR_nubar_p_NC_1Pi:NR_nubar_p_NC_2Pi:NR_nubar_p_NC_3Pi

# Can run in tmux sessions
# no syst: ~4hr
# 4 syst: ~16hr
# 9 syst: ~19hr

hadd_state_file_xsec_1_to_6.root list:MaCCQE:VecFFCCQEshape:CCQEPauliSupViaKF:MaCCRES:MvCCRES:Theta_Delta2Npi
hadd_state_file_xsec_7_to_12.root list:AhtBY:BhtBY:CV1uBY:CV2uBY:MaNCRES:MvNCRES
hadd_state_file_xsec_13_to_18.root list:FrCEx_N:FrElas_N:FrInel_N:FrAbs_N:FrPiProd_N:FrCEx_pi
hadd_state_file_xsec_19_to_24.root list:FrElas_pi:FrInel_pi:FrAbs_pi:FrPiProd_pi:BeRPA_A:BeRPA_B
hadd_state_file_xsec_25_to_30.root list:BeRPA_D:C12ToAr40_2p2hScaling_nu:C12ToAr40_2p2hScaling_nubar:E2p2h_A_nu:E2p2h_B_nu:E2p2h_A_nubar
hadd_state_file_xsec_31_to_36.root list:E2p2h_B_nubar:NR_nu_np_CC_1Pi:NR_nu_n_CC_2Pi:NR_nu_n_CC_3Pi:NR_nu_p_CC_2Pi:NR_nu_p_CC_3Pi
hadd_state_file_xsec_37_to_42.root list:NR_nu_n_NC_1Pi:NR_nu_n_NC_2Pi:NR_nu_n_NC_3Pi:NR_nu_p_NC_1Pi:NR_nu_p_NC_2Pi:NR_nu_p_NC_3Pi
hadd_state_file_xsec_43_to_48.root list:NR_nubar_n_CC_1Pi:NR_nubar_n_CC_2Pi:NR_nubar_n_CC_3Pi:NR_nubar_p_CC_1Pi:NR_nubar_p_CC_2Pi:NR_nubar_p_CC_3Pi
hadd_state_file_xsec_49_to_54.root list:NR_nubar_n_NC_1Pi:NR_nubar_n_NC_2Pi:NR_nubar_n_NC_3Pi:NR_nubar_p_NC_1Pi:NR_nubar_p_NC_2Pi:NR_nubar_p_NC_3Pi
hadd_state_file_xsec_55_to_56.root list:nuenumu_xsec_ratio:nuenuebar_xsec_ratio

hadd_state_file_flux_0_to_5.root list:flux_Nov17_0:flux_Nov17_1:flux_Nov17_2:flux_Nov17_3:flux_Nov17_4:flux_Nov17_5
hadd_state_file_flux_6_to_11.root list:flux_Nov17_6:flux_Nov17_7:flux_Nov17_8:flux_Nov17_9:flux_Nov17_10:flux_Nov17_11
hadd_state_file_flux_12_to_17.root list:flux_Nov17_12:flux_Nov17_13:flux_Nov17_14:flux_Nov17_15:flux_Nov17_16:flux_Nov17_17
hadd_state_file_flux_18_to_23.root list:flux_Nov17_18:flux_Nov17_19:flux_Nov17_20:flux_Nov17_21:flux_Nov17_22:flux_Nov17_23
hadd_state_file_flux_24_to_27.root list:flux_Nov17_24:flux_Nov17_25:flux_Nov17_26:flux_Nov17_27

(!!! NOT USABLE !!!) hadd_state_file_det_1_to_6.root list:FDRecoNumuSyst:FDRecoNueSyst:FVNumuFD:FVNueFD:RecoNCSyst:FVNumuND
hadd_state_file_det_7_to_12.root list:RecoEnergyScaleND:RecoEnergySqrtND:RecoEnergyInvSqrtND:EMRecoUncorrND:EMRecoUncorrSqrtND:EMRecoUncorrInvSqrtND
hadd_state_file_det_13_to_18.root list:ChargedHadRecoUncorrND:ChargedHadRecoUncorrSqrtND:ChargedHadRecoUncorrInvSqrtND:ERecoScaleMuLArND:ERecoScaleMuLArSqrtND:ERecoScaleMuLArInvSqrtND
hadd_state_file_det_19_to_24.root list:RecoEnergyScaleFD:RecoEnergySqrtFD:RecoEnergyInvSqrtFD:EMRecoUncorrFD:EMRecoUncorrSqrtFD:EMRecoUncorrInvSqrtFD
hadd_state_file_det_25_to_30.root list:ChargedHadRecoUncorrFD:ChargedHadRecoUncorrSqrtFD:ChargedHadRecoUncorrInvSqrtFD:ERecoScaleMuLArFD:ERecoScaleMuLArSqrtFD:ERecoScaleMuLArInvSqrtFD
hadd_state_file_det_31_to_36.root list:MuonRecoResND:EMRecoResND:ChargedHadRecoResND:MuonRecoResFD:EMRecoResFD:ChargedHadRecoResFD
hadd_state_file_det_37_to_39.root list:NuOnECCBkgSyst:NuOnENCBkgSyst:NuOnERecoEffSyst
```

2. Create fcl file for each systematic based on the template ```NuisanceSyst_Scan/Basic_PRISMPred_PlaceHolder.fcl```, then run the script below to run ```PRISMPrediction```,

```
cd /dune/app/users/weishi/PRISMAnalysis/lblpwgtools/CAFAna/PRISM/scripts/FermiGridPRISMScripts
chmod a+x PRISMPredVary1Syst.sh
./PRISMPredVary1Syst.sh
```

Now to apply the systematic shift to one specific step in the analysis, change ```(fVaryNDFDMCData ? shift : kNoShift))``` in following lines to ```kNoShift``` in ```PRISM/PredictionPRISM.cxx```,
```
L671/683: ND NC Bkg MC
L696/707: ND WL Bkg MC
L720/731: ND WS Bkg MC
L759: Calculate LC coefficient
L835/841: numu nue xsec correction
```
and change ```shift``` in following lines to ```kNoShift``` in ```PRISM/PRISMDetectorExtrapolation.cxx```,
```
L190: ND Unfold
L192: FD Smear
```
and change ```syst``` in following lines to ```kNoShift``` in ```PRISM/PRISMMCEffCorrection.cxx```,
```
L87/89/111/113: ND Eff corr
L127/130: FD Eff corr
```

I temporarily copy the changed files from another repo (no automatic script to do this in case there are future code updates).

Then run the script below (it only shifts each step above by changing ```kNoShift``` to ```shift``` and recompile and re-run ```PRISMPrediction```),

```
chmod a+x PRISMPredShift1StepND.sh
./PRISMPredShift1StepND.sh
```

3. Draw and plot,

```
# Do the above for all channels: FD_nu_numu, FD_nu_nue, FD_nub_numu, FD_nub_nue

# This makes the sigma variation plots for various component in PRISM
chmod a+x PlotManySysts.sh
./PlotManySysts.sh

# This puts many plots on one canvas and prints tables
source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.24.02/x86_64-centos7-gcc48-opt/bin/thisroot.sh
root -l -b -q MergeCanvas.C
```

#### Production with FermiGrid

Below is an example of submit a state file production job to FermiGrid with a single systematic.

```
cd PRISM/scripts/FermiGridPRISMScripts/
./FarmBuildPRISMInterps.sh -h
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/chasnip/CAF_MC_FILES_4FLAVOUR/ --no-fakedata-dials -a EVisReco --syst-descriptor list:MaCCQE -N -u  # run ND FHC only
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/chasnip/CAF_MC_FILES_4FLAVOUR/ --no-fakedata-dials -a EVisReco --syst-descriptor list:MaCCQE -F -u  # run FD FHC only
# Need to merge FD and ND into 1 job
```

Here is a bash script to submit jobs for many systematics,

```
cd /dune/app/users/weishi/PRISMAnalysis/lblpwgtools/CAFAna/PRISM/scripts/FermiGridPRISMScripts
chmod a+x StateFileManySystSubmit.sh
./StateFileManySystSubmit.sh
```

To check job status: ```jobsub_q --user weishi```

Fetch job logs: ```jobsub_fetchlog --jobid=<id> --unzipdir=<dir>```

To remove job: ```jobsub_rm --user weishi```

More usage here: https://cdcvs.fnal.gov/redmine/projects/jobsub/wiki/Using_the_Client

Once jobs are done, add ND and FD state files in tmux session as it takes a LONG time,

```
cd /dune/app/users/weishi/PRISMAnalysis/lblpwgtools/CAFAna/PRISM/scripts
tmux
hadd_state -h
hadd_state hadd_state_file_flux_xsec.root root://fndca1.fnal.gov:1094//pnfs/fnal.gov/usr/dune/persistent/users/weishi/CAFAnaInputs/StandardState/*.root
tmux detach
# To kill mysession
tmux kill-ses -t mysession  
# Need 24-48hrs to add 155 ND/FD files
mv hadd_state_file_flux_xsec.root /pnfs/dune/persistent/users/weishi/StateFiles
# A useful C++ debug tool gdb --args: https://root-forum.cern.ch/t/upper-limit-on-size-of-root-file-with-hadd/35069
```

### Starter task

1. The script ```PRISM/app/MakePRISMPredInterps``` loads the MC files and puts the MC data into histograms CAFAna can use to do an oscillation analysis. To run the script,

```
cd /dune/app/users/weishi/PRISMAnalysis/lblpwgtools/CAFAna
source build/Linux/CAFAnaEnv.sh                                 # set up the environment
# Ignore the error of can't find the example directory

cd PRISM/app
# For usage: MakePRISMPredInterps --help
MakePRISMPredInterps -o test.root -N-nu /dune/data/users/erea/CombinedNDCAFs/CAFv7Full_3E20OnAxis_PRISM_Feb21_FHC.root --bin-descriptor default --syst-descriptor "nosyst" --no-fakedata-dials -A EVisReco --UseSelection

# Run with systematics, all systematics (nuisance parameters) are in: CAFAna/fcl/PRISM/NuisanceParameters.fcl
MakePRISMPredInterps -o test_with_sys_fullMC.root -N-nu /pnfs/dune/persistent/users/chasnip/CAF_MC_FILES_4FLAVOUR/ND_FHC_v7Full_6E20OnAxis_absxPOT_Apr21.root -F-nu /dune/data/users/erea/FDCAFs/FD_FHC_nonswap.root -Fe-nu /dune/data/users/erea/FDCAFs/FD_FHC_nueswap.root --bin-descriptor default --no-fakedata-dials -A EVisReco --UseSelection --syst-descriptor list:MaCCQE:MaCCRES

# Running with many systematics can be submitted to FermiGrid, run each job with 1 systematic parameter
cd PRISM/scripts/FermiGridPRISMScripts/
./FarmBuildPRISMInterps.sh -h
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/chasnip/CAF_MC_FILES_4FLAVOUR/ --no-fakedata-dials -a EVisReco --syst-descriptor list:MaCCQE -N -u  # run ND FHC only
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/chasnip/CAF_MC_FILES_4FLAVOUR/ --no-fakedata-dials -a EVisReco --syst-descriptor list:MaCCQE -F -u  # run FD FHC only

# Need to write a bash script to do this for all flux+xsec systematics, refer to the finished jobs memory usage

# To check job status
jobsub_q --user weishi
# Job output in /pnfs/dune/persistent/users/weishi/CAFAnaInputs/StandardState/
jobsub_fetchlog --jobid=<id> --unzipdir=<dir>

# Add ND and FD state files
cd /dune/app/users/weishi/PRISMAnalysis/lblpwgtools/CAFAna/PRISM/scripts
hadd_state -h
hadd_state hadd_state_file.root /pnfs/dune/persistent/users/weishi/CAFAnaInputs/StandardState/ND_FHC.State.*.root

# Run over PRISMPrediction
```

The above command can also be put into a shell script.

In this first script, ```auto PRISM``` holds everything. It is an instance of ```PredictionPRISM``` class. ```AddNDDataLoader``` loads the ND data (this is actually MC but we treat it as if it is real ND data), this data remain unchanged. ```AddNDMCLoader``` loads the ND MC used to predict ND background. This background needs to be subtracted from ND data. ```AddFDMCLoader``` loads FD MC used to predict background at FD. This background will be added to the ND data once ND bkg is subtracted. And will finally be compared to FD data (where is this loaded???).

Current selection on FD events is done via convolutional neural network. Selection at ND is analytical. They are in ```Cuts/AnaCuts.h```.

2. The second one ```PRISM/app/PRISMPrediction``` then uses the root file made by ```MakePRISMPredInterps``` as an input to give PRISM predictions of the FD oscillated data. To run this script,

```
cd /dune/app/users/weishi/PRISMAnalysis/lblpwgtools/CAFAna
source build/Linux/CAFAnaEnv.sh                                 # set up the environment
# Ignore the error of can't find the example directory
# Get the fcl file
cd lblpwgtools/CAFAna/fcl/PRISM
wget https://raw.githubusercontent.com/weishi10141993/NeutrinoPhysics/main/Basic_NumuDisp.fcl --no-check-certificate

# Modify the input state file name

cd lblpwgtools/CAFAna/PRISM/app
PRISMPrediction ../../fcl/PRISM/Basic_NumuDisp.fcl
```

The ```PRISMPrediction``` script calls ```PredictionPRISM``` to subtract ND background and calls ```PRISMExtrapolator``` to calculate linear combination coefficients.

For implementing geometric efficiency, need to look at ```PredictionPRISM``` where ```PRISMMCEffCorrection``` and ```PRISMDetectorExtrapolation``` are relevant for calculate efficiency and extrapolate ND to FD.

The output goes to:

/pnfs/dune/persistent/users/weishi

3. The fitter ```PRISM/app/PRISM_NumuDisp_dChi2Scan``` also uses the state file produced from ```MakePRISMPredInterps```, it varies systematics and fit.

The basic config file is ```fcl/PRISM/PRISMOscScan_Grid.fcl```. Modify state file name and projection_name inside it. Before submit many jobs, try run interactively:

```
cd /dune/app/users/weishi/PRISMAnalysis/lblpwgtools/CAFAna
source build/Linux/CAFAnaEnv.sh                                 # set up the environment
# To recompile/rebuild before submit grid job:
./standalone_configure_and_build.sh --use-PRISM -u --rdb -f
# Ignore the error of can't find the example directory

# In addition to kerberos, you need to be in the DUNE Virtual Organization to access to global DUNE resources
# If you are on the DUNE collaboration list and have a Fermilab ID you should have been added automatically to the DUNE VO.
# Get a certificate from your kerberos ticket
kx509

# Need a proxy to access the grid resources. This is to be done once every 24 hours per login machine you’re using to identify yourself
export ROLE=Analysis
voms-proxy-init -rfc -noregen -voms=dune:/dune/Role=$ROLE -valid 120:00

# Running interactively before submtting grid job
cd PRISM/app
PRISM_NumuDisp_dChi2Scan /dune/app/users/weishi/PRISMAnalysis/lblpwgtools/CAFAna/build/Linux/fcl/PRISM/Ssth23_1DScan/PRISMOscScan_Grid_bin1.fcl

# If the above runs well, submit grid job
cd PRISM/scripts/FermiGridPRISMScripts
# if necessary: chmod a+x FarmCAFPRISMNodeScript.sh
./FarmCAFPRISMNodeScript.sh -c Ssth23ScanCommands.cmd

# To check job status
jobsub_q --user weishi
# Job output in /pnfs/dune/persistent/users/weishi/CAFAnaOutputs/MyOutput/
jobsub_fetchlog --jobid=<id> --unzipdir=<dir>

# Plot the final best fit contour
cd /dune/app/users/weishi/PRISMAnalysis/lblpwgtools/CAFAna/PRISM/scripts
hadd_fits <PoI> out.root <input root files>
# e.g., hadd_fits ssth23 ssth23_1D_fit.root /pnfs/dune/persistent/users/weishi/CAFAnaOutputs/MyOutput/PRISMScan_ssth23_bin*.root

# Subtract the min Chi2 to get dChi2 plot use a simple macro
wget https://raw.githubusercontent.com/weishi10141993/NeutrinoPhysics/main/Plot1DChiSqScan_XSecFluxNomComp.C --no-check-certificate
root -l Plot1DChiSqScan_XSecFluxNomComp.C # this produce a canvas remotely
```
