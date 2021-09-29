# On SBU SeaWulf

Make sure the ```~/.bashrc``` file contains these:

```
# ROOT with MathMore for MaCh3
source /gpfs/projects/McGrewGroup/yuewang/ROOT5/ROOT5/root/bin/thisroot.sh

# CERNLIB 2005
export CERN=/gpfs/projects/McGrewGroup/crfernandesv/CERNLIB
export CERN_LEVEL=2005
export CERN_ROOT=$CERN/$CERN_LEVEL
export CERNLIB=$CERN_ROOT/lib
export CERNLIBDIR=$CERNLIB
export CERNPATH=$CERNLIB
export PATH=$CERN_ROOT/bin:$PATH
export LD_LIBRARY_PATH=$CERNLIB:$LD_LIBRARY_PATH

# procmail 3.22
source /gpfs/projects/McGrewGroup/crfernandesv/Misc/procmail-3.22/setup.sh

# CVS
export PATH=/gpfs/projects/McGrewGroup/crfernandesv/CVS/bin:$PATH

# ND MC on SeaWulf
export MACH3_DATA=/gpfs/projects/McGrewGroup/kwood/OA2020_inputs/NDData
export MACH3_MC=/gpfs/projects/McGrewGroup/kwood/OA2020_inputs/NDMC_noPsyche
#export MACH3_MC=/gpfs/projects/McGrewGroup/kwood/OA2020_inputs/NDMC

module load git
module load slurm
module load gsl/2.4.0
module load cmake
module load cuda91/toolkit/9.1
module load anaconda
```

## Install MaCh3 for joint analysis code development

[First time only]

Check out the joint fit code,

```
cd /gpfs/projects/McGrewGroup/weishi
mkdir MaCh3
cd MaCh3
git clone git@github.com:weishi10141993/MaCh3.git
cd MaCh3
git checkout DBarrow_JointFit
./setup_CUDAProb.sh
./setup_niwgreweight.sh
./setup_T2KSKTools.sh
source setup.sh
make clean
make
```

Check the following,

```
bool useT2K = false;          # don't run on T2K beam
bool useSKCalibration = true; # only for the systematic you want to test
bool useATMPDDet = false;     # all other systematics set to false
```

Set the samples to run in ```configs/AtmosphericConfigs/AtmConfig.cfg```.

```
ATMPDFS = [1,2,4,5,6,8,9]
```

Link For SK atmospheric minituples and splines,

```
ln -s /gpfs/projects/McGrewGroup/jjjiang/my_MaCh3/MaCh3/inputs/skatm/SKMC ./inputs/skatm/SKMC
ln -s /gpfs/projects/McGrewGroup/jjjiang/my_MaCh3/MaCh3/inputs/skatm/SKMCSplines ./inputs/skatm/SKMCSplines
```

Create sample configs for all the ATMPD sample with the relevant sample bools, values and binning information,

```
cd configs/AtmosphericConfigs
python makeConfigs.py
```

Then run the executables.

The file (provided by Roger) that stores event-by-event weight for SK systematics on NextCloud is: ```/T2KSK/atm_minituples/SF.2021/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr.sysfriend.root```.

Download the file,

```
curl -u T2KSKReader:qJzSN-L3Nic-xNP75-YmS4m-Ak58P https://nextcloud.nms.kcl.ac.uk/remote.php/dav/files/T2KSKReader/T2KSK/atm_minituples/SF.2021/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr.sysfriend.root -o ./sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr.sysfriend.root
```


## Install MaCh3 for starter tasks

Regarding branches: the ```main``` branch is for OA2020. The ```develop``` branch is for OA2021. Dan's branch ```DBarrow_JointFit``` is for joint fit.

Reproduced result from ```main``` branch should be compared with TN395.

1. Check out the MaCh3:

```
cd /gpfs/projects/McGrewGroup/weishi/MaCh3
git clone git@github.com:t2k-software/MaCh3.git    # Require ssh keys setup with GitHub and membership of the t2k-software GitHub organization
cd MaCh3
git checkout DBarrow_JointFit                      # Dan's branch
```

2. [Optional, suggest to skip this] Set up psyche, this is not obligatory, you can use MaCh3 without psyche:

```
./setup_psyche.sh                                  # Require ssh keys setup with git.t2k.org GitLab (ask for an account if don't have one)
```

There seems to be some error as of Aug 25.
```
-- Could NOT find ROOT (missing: ROOT_DIR)
CMake Error at /gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/psychestuff/nd280SoftwarePolicy_master/modules/standardFunctions.cmake:849 (get_filename_component):
  get_filename_component called with incorrect number of arguments
Call Stack (most recent call first):
  /gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/psychestuff/psycheCore_3.44/cmake/psycheCoreND280_USE.cmake:3 (ND280_USE)
  /gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/psychestuff/psycheCore_3.44/cmake/psycheCoreConfig.cmake:1 (include)
  /gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/psychestuff/nd280SoftwarePolicy_master/modules/standardFunctions.cmake:834 (find_package)
  psycheMasterND280_USE.cmake:2 (ND280_USE)
  CMakeLists.txt:11 (include)
```

3. ```./setup_niwgreweight.sh```

4. ```./setup_T2KSKTools.sh```

5. ```source setup.sh```

6. Make a full compile:

```
make clean
make
```

7. Now symlink the relevant samples.

For SK atmospheric minituples and splines,

```
ln -s /gpfs/projects/McGrewGroup/jjjiang/my_MaCh3/MaCh3/inputs/skatm/SKMC ./inputs/skatm/SKMC
ln -s /gpfs/projects/McGrewGroup/jjjiang/my_MaCh3/MaCh3/inputs/skatm/SKMCSplines ./inputs/skatm/SKMCSplines
```

For T2K beam minituples and splines,

```
/gpfs/projects/McGrewGroup/jjjiang/my_MaCh3/MaCh3/inputs/SK_19b_13av7_fitqun20
/gpfs/projects/McGrewGroup/jjjiang/my_MaCh3/MaCh3/inputs/SK_19b_13av7_splines20
```

Create sample configs for all the ATMPD sample with the relevant sample bools, values and binning information,

```
cd configs/AtmosphericConfigs
python makeConfigs.py
```

### Starter tasks on DBarrow_JointFit branch (atmospheric)

1. Run atmospheric event rates

```
cd /gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3
```

In case you logged out from the previous session and re-login, source this to set library path (or put it into your .bashrc),

```
source setup.sh
```

For atmospheric event rates, run the following (note to set ```useT2K = false``` in ```PrintEventRate.cpp``` and recompile),

```
./AtmJointFit_Bin/PrintEventRate configs/AtmosphericConfigs/AtmConfig.cfg
```

Compare the output for unoscillated spectra to this [tab: UnOsc, CC+AtmFlux+MAQEH+PreBanff_Xsec SF](https://docs.google.com/spreadsheets/d/1z2AsWpKUhx113r9PGo1S-7_k5MQGmvfo-kTUhl3MhzE/edit#gid=1577838915). You need to change ```ATMPDFS = [1,5]``` in ```configs/AtmosphericConfigs/AtmConfig.cfg``` in order to compare the same samples in g-sheet.


2. Run atmospheric sigma variations

Run the executable ```./AtmJointFit_Bin/AtmSigmaVar configs/AtmosphericConfigs/AtmConfig.cfg```. Note to set ```useT2K = false``` in ```AtmSigmaVar.cpp``` and recompile.

The output is in $MaCh3/output.

3. Run atmospheric LLH scan

For atmospheric LLH scans, we have ```./AtmJointFit_Bin/SystLLHScan configs/AtmosphericConfigs/AtmConfig.cfg```. Note to set ```useT2K = false``` in ```SystLLHScan.cpp``` and recompile.

This takes a while (~6hrs/sample), use ```nohup ./AtmJointFit_Bin/SystLLHScan configs/AtmosphericConfigs/AtmConfig.cfg >& out_LLHScan_nohup.log &``` to keep it running even after logged out.

To check the status, use ```jobs -l``` or ```ps```.

To run the full set, make sure in ```configs/AtmosphericConfigs/AtmConfig.cfg```, the samples set as:

```
ATMPDFS = [1,2,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19]
```

There is a joint fit executable that's likely out of date: ```./AtmJointFit_Bin/JointAtmFit```.

### Starter tasks on the develop branch (OA2021)

OA2021 beam event rates:

```
./AtmJointFit_Bin/PrintEventRate configs/jointFit2020.cfg
```

and compare to event rates [here](https://github.com/t2k-software/MaCh3/blob/develop/doc/EventRates/OA2021/1stJune2021/PreBANFF.txt#L2).

### Starter tasks on the main branch (OA2020)

1. Do a joint fit with ND+Beam FD samples,

```
cd /gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3
git checkout main
./setup_niwgreweight.sh
source setup.sh
make clean
make
```

Need T2K beam mtuples and splines and ND inputs.

```
./bin/jointFit             # Default is ND + Beam FD joint fit
```

2. Run event rates and check they match OA2020

Here is the link to [OA2020 event rates](https://github.com/t2k-software/MaCh3/tree/main/doc/EventRates/OA2020).
