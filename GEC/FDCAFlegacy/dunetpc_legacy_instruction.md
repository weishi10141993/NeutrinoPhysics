# Run FD CAFMaker dunetpc legacy code (TDR version FD CAF)

## Setup

```
mkdir dunetpclegacy
cd dunetpclegacy

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh

setup larsoft v07_09_00 -q e17:prof
unsetup mrb
setup mrb v4_04_06          # need this version
mrb newDev
source /dune/app/users/weishi/dunetpclegacy/localProducts_larsoft_v07_09_00_e17_prof/setup

cd srcs

# On local laptop:
#   git clone https://github.com/weishi10141993/dunetpc.git -b feature/v07_09_00
# sync the changes in CAFMaker
cd ${LAPTOP_WORK_DIR}/dunetpc
rsync -e ssh -avSz  ./* weishi@dunegpvm13.fnal.gov:/dune/app/users/weishi/dunetpclegacy/srcs/dunetpc

# Build GEC code inside CAFMaker
cd dunetpc/dune/CAFMaker
./build.sh

# Go back to srcs and get systematics software
cd srcs    
mrb g nusystematics
cd nusystematics
git checkout remotes/origin/TDRSensProd
# Change this line in ups/product_deps: nutools v2_24_07 to nutools v2_24_05

cd srcs
mrb g systematicstools
cd systematicstools
git checkout remotes/origin/TDRSensProd

# Build the code:
cd srcs
mrb uc
cd ${MRB_BUILDDIR}       
mrb z
mrbsetenv
unsetup cmake
setup cmake v3_19_6
mrb b
```

Run legacy FD CAF maker production interactively,

```
cd dunetpclegacy/srcs/dunetpc/dune/CAFMaker
lar -c ../../fcl/dunefd/mergeana/select_ana_dune10kt_nu.fcl -n 100 root://fndca1.fnal.gov:1094/pnfs/fnal.gov/usr/dune/tape_backed/dunepro/mcc11/protodune/mc/full-reconstructed/07/51/31/11/nu_dune10kt_1x2x6_13009312_0_20181104T221530_gen_g4_detsim_reco.root

# or in screen mode:
nohup lar -c ../../fcl/dunefd/mergeana/select_ana_dune10kt_nu.fcl -T caf_legacy_10.root -n -1 root://fndca1.fnal.gov:1094/pnfs/fnal.gov/usr/dune/tape_backed/dunepro/mcc11/protodune/mc/full-reconstructed/07/86/27/57/nu_dune10kt_1x2x6_13027311_0_20181110T041119_gen_g4_detsim_reco.root >& output10.log &
```

Recompile if source code changed,

```
cd ${MRB_BUILDDIR}                   
mrb z
mrbsetenv  
unsetup cmake
setup cmake v3_19_6
mrb b
```

Relogin,

```
cd /dune/app/users/weishi/dunetpclegacy
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup larsoft v07_09_00 -q e17:prof
unsetup mrb
setup mrb v4_04_06
source /dune/app/users/weishi/dunetpclegacy/localProducts_larsoft_v07_09_00_e17_prof/setup
mrbsetenv
```

## Run jobs

This assumes you use Fermilab machines (dunegpvm*).

Once the above is compiled and runs without problem interactively, you can start to produce a tarball. First, you need to have a grid setup for the localProducts as the grid job typically runs on a different machine than your working machine,

```
cd /dune/app/users/weishi/dunetpclegacy/localProducts_larsoft_v07_09_00_e17_prof
cp setup setup-grid         # make a copy of the setup for grid job
```

then in ```setup-grid```, change ```/dune/app/users/weishi``` to the worker node working directory ```${_CONDOR_JOB_IWD}```.

Now get txt file that lists of input files and work env set up script:

```
cd /dune/app/users/weishi
wget https://raw.githubusercontent.com/weishi10141993/NeutrinoPhysics/main/GEC/FDCAFlegacy/MCC11FDBeamsim/MCC11FDBeamsim_nu_reco.txt --no-check-certificate
wget https://raw.githubusercontent.com/weishi10141993/NeutrinoPhysics/main/GEC/FDCAFlegacy/setup_FDlegacyCAF-grid.sh --no-check-certificate
```

Then make the tarball,

```
tar -czvf dunetpclegacy.tar.gz dunetpclegacy setupFDEffTarBall-grid.sh MCC11FDBeamsim_nu_reco.txt

# Check the tarball *.tar.gz is indeed created and open with: tar -xf *.tar.gz
```

Now get one of the following grid running scripts

```
wget https://raw.githubusercontent.com/weishi10141993/NeutrinoPhysics/main/GEC/FDCAFlegacy/run_FDlegacyCAF-autogrid.sh --no-check-certificate
```

Finally you can submit the job. The following submits N jobs (since we have 9914 files, N=9914 will run 1 files/job),
```
jobsub_submit -G dune -N 9914 --memory=5000MB --disk=1GB --expected-lifetime=8h --cpu=1 --resource-provides=usage_model=DEDICATED,OPPORTUNISTIC,OFFSITE --tar_file_name=dropbox:///dune/app/users/weishi/FDEff.tar.gz --use-cvmfs-dropbox -l '+SingularityImage=\"/cvmfs/singularity.opensciencegrid.org/fermilab/fnal-wn-sl7:latest\"' --append_condor_requirements='(TARGET.HAS_Singularity==true&&TARGET.HAS_CVMFS_dune_opensciencegrid_org==true&&TARGET.HAS_CVMFS_larsoft_opensciencegrid_org==true&&TARGET.CVMFS_dune_opensciencegrid_org_REVISION>=1105&&TARGET.HAS_CVMFS_fifeuser1_opensciencegrid_org==true&&TARGET.HAS_CVMFS_fifeuser2_opensciencegrid_org==true&&TARGET.HAS_CVMFS_fifeuser3_opensciencegrid_org==true&&TARGET.HAS_CVMFS_fifeuser4_opensciencegrid_org==true)' file:///dune/app/users/weishi/run_FDEffTarBall_autogrid.sh
```
