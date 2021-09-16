# Instruction for DUNE-PRISM analysis setup from DUNE FNAL machines (dunegpvm*)


## Install CAFAna Framework

```
mkdir PRISMAnalysis
cd PRISMAnalysis
git clone https://github.com/weishi10141993/lblpwgtools.git
cd lblpwgtools
git checkout feature/PRISM                 # if you don't want to be working on the ND nue selection stuff
cd CAFAna
# Build the code, the -u option rely on relevant dependencies from FNAL scisoft
./standalone_configure_and_build.sh --use-PRISM -u --rdb
# May need to set export GSL_LIB=/usr/lib64
```

Main scripts are ```PRISM/app/MakePRISMPredInterps``` and ```PRISM/app/PRISMPrediction```. The first script loads the MC files and puts the MC data into histograms CAFAna can use to do an oscillation analysis. The second then uses the root file made by the first script as an input to give PRISM predictions of the FD oscillated data.

To run the first script,

```
cd /dune/app/users/weishi/PRISMAnalysis/lblpwgtools/CAFAna
source build/Linux/CAFAnaEnv.sh                                 # set up the environment
# Ignore the error of can't find the example directory
cd PRISM/app
MakePRISMPredInterps -o test.root -N-nu /dune/data/users/erea/CombinedNDCAFs/CAFv7Full_3E20OnAxis_PRISM_Feb21_FHC.root --bin-descriptor uniform --syst-descriptor "nosyst" --no-fakedata-dials -A ETrue --UseSelection
# For usage: MakePRISMPredInterps --help
```

Or put it into a shell script,

```
#!/bin/bash
INPUTDIR=/dune/data/users/erea
LOS="nosyst"
MakePRISMPredInterps -o /dune/data/users/erea/PRISMPredInterps/v7fullMC_NDnumutoNDnue_withSel_noEhadVeto_20210811.root \
        -N-nu ${INPUTDIR}/CombinedNDCAFs/CAFv7Full_3E20OnAxis_PRISM_Feb21_FHC.root \
#:      -F-nu ${INPUTDIR}/FDCAFs/FD_FHC_nonswap.root \
#       -Fe-nu ${INPUTDIR}/FDCAFs/FD_FHC_nueswap.root \
        --bin-descriptor uniform --syst-descriptor ${LOS} --no-fakedata-dials \
        -A ETrue --UseSelection
```

In this first script, ```auto PRISM``` holds everything. It is an instance of ```PredictionPRISM``` class. ```AddNDDataLoader``` loads the ND data (this is actually MC but we treat it as if it is real ND data), this data remain unchanged. ```AddNDMCLoader``` loads the ND MC used to predict ND background. This background needs to be subtracted from ND data. ```AddFDMCLoader``` loads FD MC used to predict background at FD. This background will be added to the ND data once ND bkg is subtracted. And will finally be compared to FD data (where is this loaded???).

Current selection on FD events is done via convolutional neural network. Selection at ND is analytical. They are in ```Cuts/AnaCuts.h```.

To run the second script,

```
cd /dune/app/users/weishi/PRISMAnalysis/lblpwgtools/CAFAna
source build/Linux/CAFAnaEnv.sh                                 # set up the environment
# Ignore the error of can't find the example directory
cd PRISM/app
PRISMPrediction *.fcl
```

The ```PRISMPrediction``` script calls ```PredictionPRISM``` to subtract ND background and calls ```PRISMExtrapolator``` to calculate linear combination coefficients.

For implementing geometric efficiency, need to look at ```PredictionPRISM``` where ```PRISMMCEffCorrection``` and ```PRISMDetectorExtrapolation``` are relevant for calculate efficiency and extrapolate ND to FD.

/pnfs/dune/persistent/users/weishi
