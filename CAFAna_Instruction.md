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
cd /dune/app/users/weishi/lblpwgtools/CAFAna
source build/Linux/CAFAnaEnv.sh                                 # set up the environment
# Ignore the error of can't find the example directory

cd PRISM/app
# For usage: MakePRISMPredInterps --help
MakePRISMPredInterps -o test.root -N-nu /dune/data/users/erea/CombinedNDCAFs/CAFv7Full_3E20OnAxis_PRISM_Feb21_FHC.root --bin-descriptor uniform --syst-descriptor "nosyst" --no-fakedata-dials -A ETrue --UseSelection

# Run with systematics
# All systematics (nuisance parameters) are in: lblpwgtools/CAFAna/fcl/PRISM/NuisanceParameters.fcl
MakePRISMPredInterps -o test_with_sys.root -N-nu /dune/data/users/erea/CombinedNDCAFs/CAFv7Full_3E20OnAxis_PRISM_Feb21_FHC.root -F-nu /dune/data/users/erea/FDCAFs/FD_FHC_nonswap.root -Fe-nu /dune/data/users/erea/FDCAFs/FD_FHC_nueswap.root --bin-descriptor uniform --no-fakedata-dials -A ETrue --UseSelection --syst-descriptor list:MaCCQE:MaCCRES

# Running with 2-3 systematics/time usually takes a day, if have 50 systematics, run 50 times, each time with 1 nuisance parameter, get 50 output files, then combine into one file using /PRISM/scripts/hadd_state.C to add 50 systematics correctly. (run as executable hadd_state outfile file1 file2...)

# the 50 runs can be submitted to FermiGrid using /PRISM/scripts/FermiGridPRISMScripts/FarmBuildPRISMInterps.sh, run 50 times, each with 1 systematic parameter.

# need to use ifdh cp -D for file transfer,
```

The above command can be put into a shell script,

```
#!/bin/bash
INPUTDIR=/dune/data/users/erea
LOS="nosyst"
MakePRISMPredInterps -o /dune/data/users/erea/PRISMPredInterps/v7fullMC_NDnumutoNDnue_withSel_noEhadVeto_20210811.root \
        -N-nu ${INPUTDIR}/CombinedNDCAFs/CAFv7Full_3E20OnAxis_PRISM_Feb21_FHC.root \
        -F-nu ${INPUTDIR}/FDCAFs/FD_FHC_nonswap.root \
        -Fe-nu ${INPUTDIR}/FDCAFs/FD_FHC_nueswap.root \
        --bin-descriptor uniform --syst-descriptor ${LOS} --no-fakedata-dials \
        -A ETrue --UseSelection
```

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
# To recompile:
./standalone_configure_and_build.sh --use-PRISM -u --rdb -f
# Ignore the error of can't find the example directory

# In addition to kerberos, you need to be in the DUNE Virtual Organization to access to global DUNE resources
# If you are on the DUNE collaboration list and have a Fermilab ID you should have been added automatically to the DUNE VO.
# Get a certificate from your kerberos ticket
kx509

# Need a proxy to access the grid resources. This is to be done once every 24 hours per login machine you’re using to identify yourself
export ROLE=Analysis
voms-proxy-init -rfc -noregen -voms=dune:/dune/Role=$ROLE -valid 120:00


cd PRISM/scripts/FermiGridPRISMScripts
# if necessary: chmod a+x FarmCAFPRISMNodeScript.sh
./FarmCAFPRISMNodeScript.sh -c Ssth23ScanCommands.cmd

# to check job status
jobsub_q --user weishi
# job output in /pnfs/dune/persistent/users/weishi/
jobsub_fetchlog --jobid=<id> --unzipdir=<dir>
```
