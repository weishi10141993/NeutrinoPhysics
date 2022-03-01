# Instruction for DUNE-PRISM analysis setup from DUNE FNAL machines (dunegpvm*)

## Plots4Chris

Study 1: Distributions of FD numu reco spectra for different values of dm2 and ss2th23.  Possibly with a systematic shift (i.e. missing proton MD) applied as well.  One idea I have is showing two curves, which look nearly identical, but one has missing proton and the other has a shifted dm2.  This is not new work, but reviewers are really, really confused about PRISM and we need to try to make very simple plots. I can give you a script, but probably it will need to be modified somehow to add a syst bias.

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

# Example command:
# cafe -q -b spec_joint.C /dune/data/users/picker24/CAFAnaStateFiles/v4_all/State test.root ndfd:48 1 ssth23:0.52,dmsq32:2.52 MissingProtonFakeData_pos
# Here are what these arguments mean:
#   1: input state file
#   2: output plot file
#   3: ndfd:48 <--> file:exposure  # 48 means 48kt-MW-yrs?
#   4: 1 <--> normal mass hierarchy
#   5: Asimov points/osc parameters: https://github.com/DUNE/lblpwgtools/blob/energy_shift_studies/CAFAna/Analysis/CalcsNuFit.cxx#L90
#   6: Syst shift (fake/mock data): https://github.com/weishi10141993/lblpwgtools/blob/energy_shift_studies/CAFAna/Analysis/common_fit_definitions.cxx#L533

# Do these combinations (oct default to 1, th13?)
index ssth23 dmsq32 (unit 10^-3 eV^2)  
1     0.55   2.45                      
2     0.55   2.50                      
3     0.60   2.55                     

cafe -q -b spec_joint.C /dune/data/users/picker24/CAFAnaStateFiles/v4_all/State hists_index_1.root ndfd:48 1 ssth23:0.55,dmsq32:2.45
cafe -q -b spec_joint.C /dune/data/users/picker24/CAFAnaStateFiles/v4_all/State hists_index_2.root ndfd:48 1 ssth23:0.55,dmsq32:2.50
cafe -q -b spec_joint.C /dune/data/users/picker24/CAFAnaStateFiles/v4_all/State hists_index_3.root ndfd:48 1 ssth23:0.60,dmsq32:2.55
cafe -q -b spec_joint.C /dune/data/users/picker24/CAFAnaStateFiles/v4_all/State hists_index_1_missing_proton.root ndfd:48 1 ssth23:0.55,dmsq32:2.45 MissingProtonFakeData_pos
cafe -q -b spec_joint.C /dune/data/users/picker24/CAFAnaStateFiles/v4_all/State hists_index_2_missing_proton.root ndfd:48 1 ssth23:0.55,dmsq32:2.50 MissingProtonFakeData_pos
cafe -q -b spec_joint.C /dune/data/users/picker24/CAFAnaStateFiles/v4_all/State hists_index_3_missing_proton.root ndfd:48 1 ssth23:0.60,dmsq32:2.55 MissingProtonFakeData_pos

wget https://raw.githubusercontent.com/weishi10141993/NeutrinoPhysics/main/QuickOverlay.C --no-check-certificate
root -l -b -q QuickOverlay.C
```

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
git checkout Catastrophic_Yolo_PRISM_Merge
cd CAFAna
# Build the code, the -u option rely on relevant dependencies from FNAL scisoft
./standalone_configure_and_build.sh -u -r --db          # for new YOLO branch
# To recompile: ./standalone_configure_and_build.sh -u -r --db -f
# May need to set export GSL_LIB=/usr/lib64
```

## Produce fit contours

Newest state file with all xsec systs: ```/dune/app/users/weishi/StateFiles/PRISMState_FHC_RHC_XSecSysts_NewBin_6Feb22.root```.
Or ```root://fndca1.fnal.gov:1094//pnfs/fnal.gov/usr/dune/persistent/users/chasnip/CAFAnaStateFiles/PRISMState_FHC_RHC_XSecSysts_NewBin_6Feb22.root```.

The fitter ```PRISM/app/PRISM_4Flavour_dChi2Scan``` also uses the state file produced from ```MakePRISMPredInterps```, it varies systematics and fit.

The basic config file is ```fcl/PRISM/PRISMOscScan_Grid.fcl```. Modify state file name inside it. Before submit many jobs, try run interactively:

```
cd /dune/app/users/weishi/PRISMAnalysis/lblpwgtools/CAFAna
source build/Linux/CAFAnaEnv.sh                                 # set up the environment
# To recompile/rebuild before submit grid job:
./standalone_configure_and_build.sh -u -r --db -f
# Ignore the error of can't find the example directory

# In addition to kerberos, you need to be in the DUNE Virtual Organization to access to global DUNE resources
# If you are on the DUNE collaboration list and have a Fermilab ID you should have been added automatically to the DUNE VO.
# Get a certificate from your kerberos ticket
kx509

# Need a proxy to access the grid resources. This is to be done once every 24 hours per login machine you’re using to identify yourself
export ROLE=Analysis
voms-proxy-init -rfc -noregen -voms=dune:/dune/Role=$ROLE -valid 120:00

# Running interactively to test
cd PRISM/app
PRISM_4Flavour_dChi2Scan /dune/app/users/weishi/PRISMAnalysis/lblpwgtools/CAFAna/build/Linux/fcl/PRISM/Dmsq32_1DScan/PRISMOscScan_Grid_bin1.fcl

# If the above runs well, submit grid job
cd PRISM/scripts/FermiGridPRISMScripts
# If necessary: chmod a+x FarmCAFPRISMNodeScript.sh
./FarmCAFPRISMNodeScript.sh -c Dmsq32ScanCommands.cmd

# To check job status
jobsub_q --user weishi
# Job output in /pnfs/dune/persistent/users/weishi/CAFAnaOutputs/MyOutput/
jobsub_fetchlog --jobid=<id> --unzipdir=<dir>

# Plot the final best fit contour
cd /dune/app/users/weishi/PRISMAnalysis/lblpwgtools/CAFAna/PRISM/scripts
hadd_fits <PoI> <output> <input>
# e.g., hadd_fits dmsq32 dmsq32_1D_fit.root /pnfs/dune/persistent/users/weishi/CAFAnaOutputs/MyOutput/PRISMScan_dmsq32_bin*.root

# Subtract the min Chi2 to get dChi2 plot use a simple macro
# Modify the PoI inside
source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.24.02/x86_64-centos7-gcc48-opt/bin/thisroot.sh
root -l -b -q Plot1DChiSqScan.C
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
