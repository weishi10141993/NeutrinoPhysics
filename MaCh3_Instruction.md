# On SBU SeaWulf

Make sure the ```~/.bashrc``` file contains these:

```
# ROOT with MathMore for MaCh3
source /gpfs/projects/McGrewGroup/yuewang/ROOT5/ROOT5/root/bin/thisroot.sh

# Splines from Mo Jia
t2ksoftware_dir=/gpfs/projects/McGrewGroup/mojia/t2ksoftware/t2kreweight
source ${t2ksoftware_dir}/FSIFitter/setup_octave.sh
source ${t2ksoftware_dir}/FSIFitter/setup.sh
source ${t2ksoftware_dir}/GEANTReWeight/setup.sh

source ${t2ksoftware_dir}/neut/build/Linux/setup.sh
source ${t2ksoftware_dir}/NIWGReWeight/build/Linux/setup.sh
source ${t2ksoftware_dir}/T2KReWeight/build/Linux/setup.sh

xsecDir=/gpfs/projects/McGrewGroup/weishi/XsecResponse
export LD_LIBRARY_PATH=${xsecDir}/lib:$LD_LIBRARY_PATH

# CMT                                                                                                                                                                        
source /gpfs/projects/McGrewGroup/jjjiang/my_MaCh3/CMT/setup.sh

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
export MACH3_DATA=/gpfs/projects/McGrewGroup/jjjiang/my_MaCh3/OA2019/ND280/Splines/Data_nd280Psyche_v3r47
export MACH3_MC=/gpfs/projects/McGrewGroup/jjjiang/my_MaCh3/OA2019/ND280/Splines/MC_nd280Psyche_v3r47_ISOFIX
#export MACH3_DATA=/gpfs/projects/McGrewGroup/kwood/OA2020_inputs/NDData
#export MACH3_MC=/gpfs/projects/McGrewGroup/kwood/OA2020_inputs/NDMC_noPsyche
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
# Get a customized setup_psyche for SeaWulf
rm setup_psyche.sh
wget https://raw.githubusercontent.com/weishi10141993/NeutrinoPhysics/main/setup_psyche.sh --no-check-certificate
chmod a+x setup_psyche.sh
./setup_psyche.sh        # MaCh3 itself doesn't need this, but when fitting w/ ND stuff you will need it
source setup.sh          # Need to do this if re-login
make clean
make
```

Link T2K beam and SK atmospheric minituples and splines,

```
ln -s /gpfs/projects/McGrewGroup/jjjiang/my_MaCh3/MaCh3/inputs/SK_19b_13av7_fitqun20 ./inputs/SK_19b_13av7_fitqun20
ln -s /gpfs/projects/McGrewGroup/jjjiang/my_MaCh3/MaCh3/inputs/SK_19b_13av7_splines20 ./inputs/SK_19b_13av7_splines20
ln -s /gpfs/projects/McGrewGroup/jjjiang/my_MaCh3/MaCh3/inputs/skatm/SKMC ./inputs/skatm/SKMC
ln -s /gpfs/projects/McGrewGroup/jjjiang/my_MaCh3/MaCh3/inputs/skatm/SKMCSplines ./inputs/skatm/SKMCSplines
# If want unlink symlink: unlink SKMCSplines  (do not use rm!!!)
```

Create sample configs for all the ATMPD sample with the relevant sample bools, values and binning information,

```
cd configs/AtmosphericConfigs
python makeConfigs.py
```

Then run the executables.

```
./AtmJointFit_Bin/PrintEventRate configs/AtmosphericConfigs/AtmConfig.cfg
./AtmJointFit_Bin/CreateRCTables configs/AtmosphericConfigs/AtmConfig.cfg  # Generate new RC tables with updated evt topology
./AtmJointFit_Bin/MakeAtmDetHists configs/AtmosphericConfigs/AtmConfig.cfg
#./AtmJointFit_Bin/PlotAtmByMode configs/AtmosphericConfigs/AtmConfig.cfg 27

# Run the joint fit:
./AtmJointFit_Bin/JointAtmFit configs/AtmosphericConfigs/AtmConfig.cfg
```

## Running MaCh3 on grid

The first joint fit requires 19Gb/chain of RAM and the step time is O(1s/step). And about 2.5Gb/chain of GPU memory, regardless of systematics fixed or not (but do matter if you remove systematics).

To run on GPU: uncomment setup.sh #export CUDAPATH=${CUDA_HOME} and recompile, once it’s compiled in GPU, it can run on GPU queue

### SeaWulf

Send slurm jobs on SeaWulf. Example script ```SlurmRunMCMCChain0-4Job1.sh``` running on CPU (8 cores per chain, 5 chains on 1 node):
It doesn't work to request 2 nodes and run 10 chains. So better to put 5 chains into each job, each on a node. Max number of nodes you can request: https://it.stonybrook.edu/help/kb/seawulf-queues
The following 5 chains run time 1-11:41:38
```
#!/bin/bash
#
#SBATCH --job-name=MaCh3Fit
#SBATCH --output=MaCh3Fit.log
#SBATCH --mail-type=END,FAIL
#SBATCH --mail-user=wei.shi.1@stonybrook.edu
#SBATCH --nodes=1
#SBATCH --time=168:00:00
#SBATCH -p extended-40core

JOBNUM=1

cd /gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3

export OMP_NUM_THREADS=8

for CHAINNUM in 0 1 2 3 4
do
  ./AtmJointFit_Bin/JointAtmFit configs/AtmosphericConfigs/job${JOBNUM}/AtmConfig_ch${CHAINNUM}.cfg > /gpfs/scratch/weishi2/job${JOBNUM}/ch${CHAINNUM}.out &
  sleep 5
done
wait
```

```
# Recommend as much resource as possible: spline evaluation dominate resource and can't be done by GPU
sbatch SlurmRunMCMCChain0-4Job1.sh        
sbatch SlurmRunMCMCChain5-9Job2.sh

# if want to submit separate 10 jobs
# Check job status: squeue --user=weishi2
# Job output
```

### Aspen/Birch/Cedar/Fir

### Chain diagnosis

You can run diagnosis while the chain is still running
```
# Diagnose
./AtmJointFit_Bin/DiagMCMC ./output/MaCh3-Atmospherics-MCMC.root

# Dump autocorrelation plot into pdf
root -l 'PlotAutoCorrelations.C("/gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3/output/MaCh3-Atmospherics-MCMC_MCMC_diag.root", "Auto_corr")'
```

The file (provided by Roger) that stores event-by-event weight for SK systematics on NextCloud is: ```/T2KSK/atm_minituples/SF.2021/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr.sysfriend.root```.

Download the file,

```
curl -u T2KSKReader:qJzSN-L3Nic-xNP75-YmS4m-Ak58P https://nextcloud.nms.kcl.ac.uk/remote.php/dav/files/T2KSKReader/T2KSK/atm_minituples/SF.2021/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr.sysfriend.root -o ./sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr.sysfriend.root

curl -u ASGReader:mkND3-2k6PP-dwyM2-8coi2-rnsRR https://nextcloud.nms.kcl.ac.uk/remote.php/dav/files/ASGReader/ASG/asg_backup/asg/asg2019oa/ND280/Splines/ -o .
```
### Make plots from chain


## Spline production

Install the following softwares:
NEUT: matrix dial
niwgreweight: adler angle dial
T2KReweight: interface
OAGenWeightsApps: calc weight
XsecResponse: prod spline

```
# Source Mo's setup on NEUT, niwgreweight, T2KReweight
git clone git@github.com:t2k-software/OAGenWeightsApps.git -b DBarrow_JointFit
cd OAGenWeightsApps
mkdir build
cd build
cmake ../ -DUSE_SK=ON
make
make install
cd Linux/bin/
source ../setup.sh

# Produce a weight file for a sample and a specific channel
genWeights_T2KSKAtm_OA2020_AdlerAngle_MatrixElement_SIPN_CRPA -s /gpfs/projects/McGrewGroup/jjjiang/my_MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample2_Channel1.root -o /gpfs/projects/McGrewGroup/weishi/test.root
```

```
git clone git@github.com:t2k-software/XsecResponse.git -b DBarrow_JointFit
cd XsecResponse
make
export LD_LIBRARY_PATH=$PWD/lib:$LD_LIBRARY_PATH
bin/make_xsec_response_sk_2019_2d -w /gpfs/projects/McGrewGroup/weishi/test.root -m /gpfs/projects/McGrewGroup/jjjiang/my_MaCh3/MaCh3/inputs/skatm/SKMC/sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr_Sample2_Channel1.root -o /gpfs/projects/McGrewGroup/weishi/splines_test.root -s 2
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
ln -s /gpfs/projects/McGrewGroup/jjjiang/my_MaCh3/MaCh3/inputs/SK_19b_13av7_fitqun20 ./inputs/SK_19b_13av7_fitqun20
ln -s /gpfs/projects/McGrewGroup/jjjiang/my_MaCh3/MaCh3/inputs/SK_19b_13av7_splines20 ./inputs/SK_19b_13av7_splines20
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
