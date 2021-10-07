# Instruction for DUNE-PRISM analysis setup from DUNE FNAL machines (dunegpvm*)


## Install CAFAna Framework

[First time only]

```
cd /dune/app/users/weishi
mkdir PRISMAnalysis
cd PRISMAnalysis
git clone https://github.com/weishi10141993/lblpwgtools.git
cd lblpwgtools
git checkout feature/PRISM
cd CAFAna
# Build the code, the -u option rely on relevant dependencies from FNAL scisoft
./standalone_configure_and_build.sh --use-PRISM -u --rdb
# To recompile:
./standalone_configure_and_build.sh --use-PRISM -u --rdb -f
# May need to set export GSL_LIB=/usr/lib64
```

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
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/chasnip/CAF_MC_FILES_4FLAVOUR/ --no-fakedata-dials -a EVisReco --syst-descriptor list:MaCCQE
./FarmBuildPRISMInterps.sh -i /pnfs/dune/persistent/users/chasnip/CAF_MC_FILES_4FLAVOUR/ --no-fakedata-dials -a EVisReco --syst-descriptor list:MaCCRES
# Need to write a bash script to do this for many systematics

# To check job status
jobsub_q --user weishi
# Job output in /pnfs/dune/persistent/users/weishi/CAFAnaInputs/StandardState/
jobsub_fetchlog --jobid=<id> --unzipdir=<dir>

# Combine into one file (run as executable hadd_state outfile file1 file2...)
cd /dune/app/users/weishi/PRISMAnalysis/lblpwgtools/CAFAna/PRISM/scripts
hadd_state -h
hadd_state <PoI> out.root <input root files>
# e.g., hadd_state hadd_state_file.root /pnfs/dune/persistent/users/weishi/CAFAnaInputs/StandardState/ND_FHC.State.*.root
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
