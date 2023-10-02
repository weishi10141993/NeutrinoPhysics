Notes:

# New state prod with half had bins

## Stat only

```
# Asimov-like fit: ssth23_dmsq32
Nue_app: DONE
FourFlavor: DONE

# Fake data fit
FakeDataFourFlavor: DONE
FakeDataNuBarModeJoint: DONE
FakeDataDispJoint: DONE
FakeDataNumu_disp: DONE
FakeDataNumubar_disp: DONE
FakeDataNue_app: DONE
FakeDataNuebar_app: DONE

# Above joint fit excludes nutau
Nutau_app?
Nutaubar_app?

FakeDataNuBarModeJoint/FakeDataDispJoint: with double EHad bins, saw filled min point 1e-6, Ciaran found for negative pred, chi2 failed, need to fix how chi2 is calculated, file 27_11/12/13
```

## Flux Syst

```
# Input flux shift file: flux_shifts_OffAxis.root as of Jan 18, 2023
CAFAna numbering scheme:
flux_Nov17_0  - BeamOffsetX
flux_Nov17_1  - BeamSigma
flux_Nov17_2  - BeamThetaPhi
flux_Nov17_3  - BeamTheta
flux_Nov17_4  - DPR
flux_Nov17_5  - Horn1XShift
flux_Nov17_6  - Horn1YShift
flux_Nov17_7  - Horn2XShift
flux_Nov17_8  - Horn2YShift
flux_Nov17_9  - HornCurrent
flux_Nov17_10 - POTCounting
flux_Nov17_11 - TargetDensity
flux_Nov17_12 - WL

# Input flux shift file: flux_shifts_OffAxis2022.root
will be labelled by Sept21, not ready to use yet as of Jan 18, 2023
```

```
# 25 flux syst, FourFlavor fit
# Beam focusing 0-12
flux_Nov17_0: DONE
flux_Nov17_1: DONE
flux_Nov17_2: DONE
flux_Nov17_3: DONE
flux_Nov17_4: DONE
flux_Nov17_5: DONE
flux_Nov17_6: DONE
flux_Nov17_7: DONE
flux_Nov17_8: DONE
flux_Nov17_9: DONE
flux_Nov17_10: DONE
flux_Nov17_11: DONE
flux_Nov17_12: DONE

# Hadron production: 13-24
flux_Nov17_13: DONE
flux_Nov17_14: DONE
flux_Nov17_15: FAIL
flux_Nov17_16: FAIL
flux_Nov17_17
flux_Nov17_18
flux_Nov17_19
flux_Nov17_20
flux_Nov17_21
flux_Nov17_22
flux_Nov17_23
flux_Nov17_24

./FarmCAFPRISMNodeScript.sh -c PRISM_2DScan_Commands.cmd
```

Fit with Ciaran's EnuReco file:
```
stat only: 678600 DONE
68225100.0 flux_Nov17_0 DONE
68225663.0 flux_Nov17_5 DONE
```

Produce state file with single syst:
```
# ND FHC
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/chasnip/NDCAF_OnAxisHadd/FHC --no-fakedata-dials -a EVisReco --bin-descriptor prism_default --syst-descriptor "list:flux_Nov17_0" -N -u

# ND RHC
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/chasnip/NDCAF_OnAxisHadd/RHC --no-fakedata-dials -a EVisReco --bin-descriptor prism_default --syst-descriptor "list:flux_Nov17_0" -N -b

# FD FHC COPY OLD
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/weishi/FDCAF/FHC/nonswap --no-fakedata-dials -a EVisReco --bin-descriptor prism_default --syst-descriptor "list:flux_Nov17_0" -F -u
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/weishi/FDCAF/FHC/nueswap --no-fakedata-dials -a EVisReco --bin-descriptor prism_default --syst-descriptor "list:flux_Nov17_0" -F -u
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/weishi/FDCAF/FHC/tauswap --no-fakedata-dials -a EVisReco --bin-descriptor prism_default --syst-descriptor "list:flux_Nov17_0" -F -u

# FD RHC
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/weishi/FDCAF/RHC/nonswap --no-fakedata-dials -a EVisReco --bin-descriptor prism_default --syst-descriptor "list:flux_Nov17_0" -F -b
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/weishi/FDCAF/RHC/nueswap --no-fakedata-dials -a EVisReco --bin-descriptor prism_default --syst-descriptor "list:flux_Nov17_0" -F -b
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/weishi/FDCAF/RHC/tauswap --no-fakedata-dials -a EVisReco --bin-descriptor prism_default --syst-descriptor "list:flux_Nov17_0" -F -b

# Add state files
./FarmHaddCafanaGrid.sh -i /pnfs/dune/scratch/users/weishi/DebugFluxFit/EVisReco_prism_default/flux_Nov17_0/NDFHC_batch1  
./FarmHaddCafanaGrid.sh -i /pnfs/dune/scratch/users/weishi/DebugFluxFit/EVisReco_prism_default/flux_Nov17_0/NDFHC_batch2  
./FarmHaddCafanaGrid.sh -i /pnfs/dune/scratch/users/weishi/DebugFluxFit/EVisReco_prism_default/flux_Nov17_0/NDFHC_batch3  
./FarmHaddCafanaGrid.sh -i /pnfs/dune/scratch/users/weishi/DebugFluxFit/EVisReco_prism_default/flux_Nov17_0/FD            
./FarmHaddCafanaGrid.sh -i /pnfs/dune/scratch/users/weishi/DebugFluxFit/EVisReco_prism_default/flux_Nov17_0/NDRHC_batch1  
./FarmHaddCafanaGrid.sh -i /pnfs/dune/scratch/users/weishi/DebugFluxFit/EVisReco_prism_default/flux_Nov17_0/NDRHC_batch2  
./FarmHaddCafanaGrid.sh -i /pnfs/dune/scratch/users/weishi/DebugFluxFit/EVisReco_prism_default/flux_Nov17_0/NDRHC_batch3  

./FarmHaddCafanaGrid.sh -i /pnfs/dune/persistent/users/weishi/DebugFluxFit/EVisReco_prism_default/flux_Nov17_0/Hadd
```

Produce state file with all flux systs (flux_Nov17_<0-24>):
```
cd PRISM/scripts/FermiGridPRISMScripts

# ND FHC
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/chasnip/NDCAF_OnAxisHadd/FHC --no-fakedata-dials -a ELepEHadVisReco --bin-descriptor lep_default --syst-descriptor "noxsec:nodet" -N -u

# ND RHC
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/chasnip/NDCAF_OnAxisHadd/RHC --no-fakedata-dials -a ELepEHadVisReco --bin-descriptor lep_default --syst-descriptor "noxsec:nodet" -N -b

# FD FHC  
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/weishi/FDCAF/FHC/nonswap --no-fakedata-dials -a ELepEHadVisReco --bin-descriptor lep_default --syst-descriptor "noxsec:nodet" -F -u
# this runs over nonswap+nueswap as nonswap always need to be included (not clear why)
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/weishi/FDCAF/FHC/nueswap --no-fakedata-dials -a ELepEHadVisReco --bin-descriptor lep_default --syst-descriptor "noxsec:nodet" -F -u
# this runs over nonswap+tauswap as nonswap always need to be included (not clear why)
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/weishi/FDCAF/FHC/tauswap --no-fakedata-dials -a ELepEHadVisReco --bin-descriptor lep_default --syst-descriptor "noxsec:nodet" -F -u

# FD RHC
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/weishi/FDCAF/RHC/nonswap --no-fakedata-dials -a ELepEHadVisReco --bin-descriptor lep_default --syst-descriptor "noxsec:nodet" -F -b
# this runs over nonswap+nueswap as nonswap always need to be included (not clear why)
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/weishi/FDCAF/RHC/nueswap --no-fakedata-dials -a ELepEHadVisReco --bin-descriptor lep_default --syst-descriptor "noxsec:nodet" -F -b
# this runs over nonswap+tauswap as nonswap always need to be included (not clear why)
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/weishi/FDCAF/RHC/tauswap --no-fakedata-dials -a ELepEHadVisReco --bin-descriptor lep_default --syst-descriptor "noxsec:nodet" -F -b
```

Hadd the output state files:

```
./FarmHaddCafanaGrid.sh -i /pnfs/dune/persistent/users/weishi/CAFAnaInputs/StandardState/ELepEHadVisReco_lep_default_HalfHadbins/fluxsyst_Nov17/Hadded
```

## How to Hadd ND CAF files produced by grid/production team

The main app is ```PRISM/app/OffAxisNDCAFCombiner```, it works out the file exposure of before and after hadding.

```
./BuildOffAxisInputs.sh
```

The number of files is summarized in each ND position is summarized in ```OffAxisPoTCalc.C```. It produces ```TotalOffAxisFileExposure.root``` that will combine with the weight contained in CAF. ```m-293``` in the root file refers to RHC mode.

NEED TO ADD GEC RANDOM THROW TREE.

## How to add new flux systematics based on provided beam MC
Beam MC will contain nominal, +/-1sigma flux distributions of all flux systematics.

```
PRISM/scripts/produce_flux_syst_inputs.C
```
will get the fractional flux shift --> output: flux_shift_OffAxis_2022.root (79 ND off-axis slices now)

The class in ```Syst/NewOffAxisFluxUncertainty2022Helper.cxx``` will then take this file and propagate to analysis the shift.

```
Syst/DUNEFluxSysts.h (ISyst)
```
will then use the class NewOffAxisFluxUncertainty2022Helper to get shifts.

## NC bkg pred
PredictionPRISM.cxx: GetGaussian
NC ND --> FD NC bkg

## Group of systematics impact on fit contours

The general objective is that user will specify a set of systematics of interest and remove them during the fit to study its impact on the sensitivity. Also the impact on each analysis step will be included.

1. First run the following script to do poi (```dmsq32``` or ```ssth23```) fit with all N systs included (N can be: xsec_all, flux_all, flux + xsec, det_all, flux + xsec + det), then only does syst shift at one of the following steps of the analysis and refit to evaluate impacts on the fit from the excluded systs.

```
cd /dune/app/users/weishi/PRISMAnalysis/lblpwgtools/CAFAna/PRISM/scripts/FermiGridPRISMScripts
# Make sure to update poi options in the script: <dmsq32, ssth23>
chmod a+x FitAllSyst.sh

# !!! Submit jobs in tmux session !!!
./FitAllSyst.sh > log_xsec_all.txt &
# or nohup

# Job output in /pnfs/dune/persistent/users/weishi/*
jobsub_fetchlog --jobid=<id> --unzipdir=<dir>

# you can run 50 points in one job now using PRISMOscScan_Grid.fcl
# change scansEG to scans
# change dmsq23_coarse to dmsq23
# instead of running 50 fcl files
```

[The following is some explanation, you can skip this part]

The relevant lines and file names for all analysis components are listed here:

```
Index   Step/component     File (.cxx)                  Line numbers
1       FD Flux Corr       PredictionPRISM              720
2       FD NuTauCC Bkg     PredictionPRISM              973
3       FD Intrinsic Bkg   PredictionPRISM              1003
4       FD WS Bkg          PredictionPRISM              988
5       FD NC Bkg          PredictionPRISM              941
6       FD WL Bkg          PredictionPRISM              959
7       ND WS Bkg MC       PredictionPRISM              695/706
8       ND NC Bkg MC       PredictionPRISM              646/658
9       ND WL Bkg MC       PredictionPRISM              671/682
10      Calculate LCWgt    PredictionPRISM              732
11      ND Unfold          PRISMDetectorExtrapolation   162
12      ND Eff corr        PRISMMCEffCorrection         111/114/128/131
13      FD Eff corr        PRISMMCEffCorrection         142/146
14      FD Smear           PRISMDetectorExtrapolation   165
15      numu-e xsec corr   PredictionPRISM              797/802         # only for appearance analysis
```

To apply the systematic shift to one specific step in the analysis, it needs to first switch off systematic shifts in all analysis components listed above.

Basically, you need to change ```(fVaryNDFDMCData ? kNoShift : shift)``` to ```kNoShift``` in ```PRISM/PredictionPRISM.cxx```,

and change ```shift``` to ```kNoShift``` in ```PRISM/PRISMDetectorExtrapolation.cxx```,

and change ```syst``` to ```kNoShift``` in ```PRISM/PRISMMCEffCorrection.cxx```.

In the script ```FitAllSyst.sh```, it temporarily copies files from another repo with all syst shift switched off (no automatic script for this as the code is constantly changing).

```
wget https://raw.githubusercontent.com/weishi10141993/NeutrinoPhysics/main/PredictionPRISM.cxx --no-check-certificate
wget https://raw.githubusercontent.com/weishi10141993/NeutrinoPhysics/main/PRISMMCEffCorrection.cxx --no-check-certificate
wget https://raw.githubusercontent.com/weishi10141993/NeutrinoPhysics/main/PRISMDetectorExtrapolation.cxx --no-check-certificate
```
It then switches on the single step.

2. Based on all systs used above, say now we want to check how the fit contour changes when n systematics (n < N) are excluded, run the script below.

```
# scp
cd /dune/app/users/weishi/PRISMAnalysis/lblpwgtools/CAFAna/PRISM/scripts/FermiGridPRISMScripts
# Make sure to update poi options: dmsq32, ssth23
# Make sure to add the systs you want to exclude
chmod a+x FitExludeSystGroup.sh

# !!! Recompile and submit jobs in tmux session !!!
./FitExludeSystGroup.sh > log_xsec_all_exclude.txt &
```

[The following is some explanation, you can skip this part]

If jobs are held due to memory or time, release it with additional resources, or use [autorelease](https://cdcvs.fnal.gov/redmine/projects/fife/wiki/Job_autorelease) when submitting the job:
```
# this adds extra 1024MB and 1800s
jobsub_release -G <group> --user <userid> --constraint 'FERMIHTC_GraceMemory==1024' --constraint 'FERMIHTC_GraceLifetime==1800'

# hold codes: https://mu2ewiki.fnal.gov/wiki/ErrorRecovery
6 sub 0,2 could not execute glidein, and/or docker did not run
9 not enough memory; remember that you need to include the size of the code tarball/release
12 sub 2 could not execute glidein
13
26 sub 8 wall time (also in the Docker era)
26 sub 1 memory limits
26 sub 4 SYSTEM_PERIODIC_HOLD  Starts/limit 31/10 - too many restarts?
28 sub -10000,512,768,256 sandbox
30 sub -10000,768 Job put on hold by remote host,  job proxy is not valid
34 sub 0 - memory limits (also in the Docker era)
35 sub 0 Error from slot - probably a condor (Docker?) failure on the worker node
??? disk limits - probably 26 sub ?

# or remove and resubmit
jobsub_rm --jobid=<id of your grid process>
--memory="nnnnMB"
```

The script ```FitExludeSystGroup.sh``` is similar to previous step. It first excludes ```n``` systs, and only fit with ```N-n``` systs. The script will find the ```n``` systematics and put ```#``` in front of it in the fcl file. Then it will recompile and resubmit job.

Then for the same ```N-n``` systs, it only does syst shift at one of the steps of the analysis listed previously, recompile and resubmit job.

3. Run the script below to plot the fitted result from N systs and N-n systs (plus result for each step).

```
cd /dune/app/users/weishi/PRISMAnalysis/lblpwgtools/CAFAna/PRISM/scripts/FermiGridPRISMScripts
# update poi options in GetFitdChi2Comp.sh: <dmsq32, ssth23>
chmod a+x GetFitdChi2Comp.sh
./GetFitdChi2Comp.sh
```

[The following is some explanation, you can skip this part]

The script first hadd all Chi2 files from finished jobs to get the Chi2 plot ```hadd_fits <PoI> <output> <input>```.

It then subtracts the min Chi2 to get dChi2 plot and compare the N-n case with N case ```root -l -b -q PlotdChiSq1DScan.C```.

Finally, it puts all dChi2 comparison plots into one canvas ```root -l -b -q MergePlots.C```.

## Plots4Chris

### Study 2

Plot dm2 resolution vs. exposure with the missing proton bias added in quadrature. Focus on short exposure.

File to calculate: 1) dm2 resolution vs exposure; 2) resolution vs true dm:

```
/pnfs/dune/persistent/users/callumw/low_exposure_sensitivities/CAFAna_throws_ndfd_*ktMWyr_NH_th13.root (*: 6, 12, 24, 66, 100)
```

To access missing proton bias (a constant) for each exposure:
```
/pnfs/dune/persistent/users/callumw/dm2_asimovs_for_wei/asimov_dmsq32_ndfd*_allsyst_th13_asimov0_hie1_MissingProtonFakeData:1.root
```

Here is the macro to get the plots:
```
cd /dune/app/users/weishi/Plots4Chris/dm2resolution
wget --no-check-certificate
source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.24.02/x86_64-centos7-gcc48-opt/bin/thisroot.sh
root -l -b -q dm2res.C
```

If want a comparison of the 1D asimovs, look at these nominal files:
```
/pnfs/dune/persistent/users/callumw/dm2_asimovs_for_wei/asimov_dmsq32_ndfd6_allsyst_th13_asimov0_hie1_nominal.root
/pnfs/dune/persistent/users/callumw/dm2_asimovs_for_wei/asimov_dmsq32_ndfd12_allsyst_th13_asimov0_hie1_nominal.root
/pnfs/dune/persistent/users/callumw/dm2_asimovs_for_wei/asimov_dmsq32_ndfd24_allsyst_th13_asimov0_hie1_nominal.root
/pnfs/dune/persistent/users/callumw/dm2_asimovs_for_wei/asimov_dmsq32_ndfd66_allsyst_th13_asimov0_hie1_nominal.root
/pnfs/dune/persistent/users/callumw/dm2_asimovs_for_wei/asimov_dmsq32_ndfd100_allsyst_th13_asimov0_hie1_nominal.root
```

### Study 1

Plot distributions of FD numu reco spectra for different values of dm2 and ss2th23 with systematic shift applied.

```
git clone https://github.com/weishi10141993/lblpwgtools.git
cd lblpwgtools
git checkout energy_shift_studies
cd CAFAna
./standalone_configure_and_build.sh -u -r
# To recompile: ./standalone_configure_and_build.sh -u -r -f

cd /dune/app/users/weishi/Plots4Chris/lblpwgtools/CAFAna
source build/Linux/CAFAnaEnv.sh
cd scripts
```

Example command:
```
# cafe -q -b spec_joint.C /dune/data/users/picker24/CAFAnaStateFiles/v4_all/State test.root ndfd:48 1 ssth23:0.52,dmsq32:2.52 MissingProtonFakeData_pos
```

Here are what these arguments mean:
```
#   1: input state file
#   2: output plot file
#   3: ndfd:48 <--> file:exposure  # 48 means 48kt-MW-yrs?
#   4: 1 <--> normal mass hierarchy
#   5: Asimov points/osc parameters: https://github.com/DUNE/lblpwgtools/blob/energy_shift_studies/CAFAna/Analysis/CalcsNuFit.cxx#L90
#   6: Syst shift (fake/mock data): https://github.com/weishi10141993/lblpwgtools/blob/energy_shift_studies/CAFAna/Analysis/common_fit_definitions.cxx#L533
```

Do these combinations (oct default to 1, th13?):
```
index ssth23 dmsq32 (unit 10^-3 eV^2)  
1     0.55   2.45                      
2     0.55   2.50                      
3     0.60   2.55                     
```

```
cafe -q -b spec_joint.C /dune/data/users/picker24/CAFAnaStateFiles/v4_all/State hists_index_1.root ndfd:48 1 ssth23:0.55,dmsq32:2.45
cafe -q -b spec_joint.C /dune/data/users/picker24/CAFAnaStateFiles/v4_all/State hists_index_2.root ndfd:48 1 ssth23:0.55,dmsq32:2.50
cafe -q -b spec_joint.C /dune/data/users/picker24/CAFAnaStateFiles/v4_all/State hists_index_3.root ndfd:48 1 ssth23:0.60,dmsq32:2.55
cafe -q -b spec_joint.C /dune/data/users/picker24/CAFAnaStateFiles/v4_all/State hists_index_1_missing_proton.root ndfd:48 1 ssth23:0.55,dmsq32:2.45 MissingProtonFakeData_pos
cafe -q -b spec_joint.C /dune/data/users/picker24/CAFAnaStateFiles/v4_all/State hists_index_2_missing_proton.root ndfd:48 1 ssth23:0.55,dmsq32:2.50 MissingProtonFakeData_pos
cafe -q -b spec_joint.C /dune/data/users/picker24/CAFAnaStateFiles/v4_all/State hists_index_3_missing_proton.root ndfd:48 1 ssth23:0.60,dmsq32:2.55 MissingProtonFakeData_pos

wget https://raw.githubusercontent.com/weishi10141993/NeutrinoPhysics/main/QuickOverlay.C --no-check-certificate
root -l -b -q QuickOverlay.C
```

## State file production with FermiGrid

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

## Add state files
FHC state files with all the detector systs here: ```/pnfs/dune/persistent/users/chasnip/ND_FHC_States_FluxSysts_Jan2022```

They are copied to ```/pnfs/dune/persistent/users/weishi/FromCiaranNDFHCStateFilesFluxJan22```

```
cd /dune/app/users/weishi/PRISMAnalysis/lblpwgtools/CAFAna
source build/Linux/CAFAnaEnv.sh                                 # set up the environment
# Ignore the error of can't find the example directory

cd bin
hadd_cafana -h
# hadd_cafana will convert normal file path into an xrootd url
# have had more success running hadd_cafana on about 100 files at a time
hadd_cafana NDFHCStateFlux.root /pnfs/dune/persistent/users/weishi/FromCiaranNDFHCStateFilesFluxJan22/ND_FHC.State*.root
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

# if want to do missing proton MD, use option: --PRISM-fake-data "MissingProtonFakeData_pos"

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
hadd_cafana -h
hadd_cafana hadd_state_file.root /pnfs/dune/persistent/users/weishi/CAFAnaInputs/StandardState/ND_FHC.State.*.root
# hadd_state is sometimes used when many systs are included

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

3. The fitter ```PRISM/app/PRISM_4Flavour_dChi2Scan``` also uses the state file produced from ```MakePRISMPredInterps```, it varies systematics and fit.

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
PRISM_4Flavour_dChi2Scan /dune/app/users/weishi/PRISMAnalysis/lblpwgtools/CAFAna/build/Linux/fcl/PRISM/Ssth23_1DScan/PRISMOscScan_Grid_bin1.fcl

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
