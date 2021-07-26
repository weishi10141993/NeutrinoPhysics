# On SeaWulf

Make sure the ```~/.bashrc ``` file contains these:

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

module load git
module load slurm
module load gsl/2.4.0
module load cmake
module load cuda91/toolkit/9.1
```

## Install MaCh3 DBarrow_JointFit branch

1. Checkout the MaCh3 branch named DBarrow_JointFit:

```
git clone git@github.com:t2k-software/MaCh3.git    # Require ssh keys setup with GitHub and membership of the t2k-software GitHub organisation
git checkout DBarrow_JointFit
```

2. Set up psyche, this is not obligatory, you can use MaCh3 without psyche:

```
./setup_psyche.sh                                  # Require ssh keys setup with git.t2k.org GitLab (ask for an account if don't have one)
```

3. ```./setup_niwgreweight.sh```

4. ```./setup_T2KSKTools.sh```

5. ```source setup.sh```

6. Make a full compile:

```
make clean
make
```

## Print event rates study

SK minituple files: ```/gpfs/projects/McGrewGroup/jjjiang/my_MaCh3/MaCh3/inputs/skatm/SKMC```
SK spline files: ```/gpfs/projects/McGrewGroup/jjjiang/my_MaCh3/MaCh3/inputs/skatm/SKMCSplines```

```
cd /gpfs/projects/McGrewGroup/weishi/MaCh3/MaCh3
```

In case you logged out from the previous session and re-login, source this to set library path (or put it into your .bashrc),

```
source setup.sh
```

Now symlink the relevant minituples and splines,

```
ln -s /gpfs/projects/McGrewGroup/jjjiang/my_MaCh3/MaCh3/inputs/skatm/SKMC ./inputs/skatm/SKMC   
ln -s /gpfs/projects/McGrewGroup/jjjiang/my_MaCh3/MaCh3/inputs/skatm/SKMCSplines ./inputs/skatm/SKMCSplines
```

Now create sample configs for all the ATMPD sample with the relevant sample bools, values and binning information,

```
python makeConfigs.py
```

Now run the following (AtmConfig.cfg is the fit config),

```
./AtmJointFit_Bin/PrintEventRate configs/AtmosphericConfigs/AtmConfig.cfg  
```

The output is in ```PrintEventRate_output.txt``` [here](https://github.com/weishi10141993/NeutrinoPhysics/blob/main/PrintEventRate_output.txt). Compare printed event rates to https://docs.google.com/spreadsheets/d/1z2AsWpKUhx113r9PGo1S-7_k5MQGmvfo-kTUhl3MhzE/edit?usp=sharing
