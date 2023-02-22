# Instruction for running GEC on DUNE FNAL machines (dunegpvm*)

## DUNE machines (dunegpvm*)

Download these files
```
mkdir test4baron
cd test4baron

wget https://raw.githubusercontent.com/weishi10141993/NeutrinoPhysics/main/GEC/BaronNDCode/new_hadron_muon_mktree.py
wget https://raw.githubusercontent.com/weishi10141993/NeutrinoPhysics/main/GEC/BaronNDCode/setup_NDcombEff.sh
wget https://raw.githubusercontent.com/weishi10141993/NeutrinoPhysics/main/GEC/BaronNDCode/run_NDcombEff.sh
wget https://raw.githubusercontent.com/weishi10141993/NeutrinoPhysics/main/GEC/BaronNDCode/NDCAFs.txt

# A list of these files are also available at: /dune/app/users/weishi/test4baron
# Make sure do a vimdiff to see what changed in other files compared to those your normally ran and update your files accordingly.

# You will still need your muonEff30.nn and muonEffModel.py. These two files are not changed.
wget https://raw.githubusercontent.com/weishi10141993/NeutrinoPhysics/main/GEC/BaronNDCode/muonEff30.nn
wget https://raw.githubusercontent.com/weishi10141993/NeutrinoPhysics/main/GEC/BaronNDCode/muonEffModel.py

# Note the last text file lists all input ND CAFs with xrootd url prefix: root://fndca1.fnal.gov:1094//pnfs/fnal.gov/usr/
# Otherwise the grid nodes won't be able to access the file.
```

### Files I/O on DUNE machines

Input ND CAF files are here: ```/pnfs/dune/persistent/physicsgroups/dunelbl/abooth/PRISM/Production/Simulation/ND_CAFMaker/v7/CAF```

Output files from grid jobs are written to the scratch area ```/pnfs/dune/scratch/users/<your username>```.

Please avoid reading from, copying from, or writing massive amount of files directly to the pnfs area ```/pnfs/dune/persistent```, this will slow down the file system. Refer to [this wiki](https://mu2ewiki.fnal.gov/wiki/DataTransfer) for good practices on data transfer.

### Interactive run

If you want to run code interactively on ```dunegpvm*``` for debugging, follow instruction in this section.

If you want to submit job from ```dunegpvm*```, go to the next section[Submit a job](#submit-a-job):
```
# This allows using pip (and do it every time you login)
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup dunetpc v09_41_00_02 -q e20:prof

# Only do this once, specify python dependencies install dir (otherwise it defaults to ~/.local/lib/python3.9/site-packages/, not enough quota)
pip install --target=/dune/app/users/weishi/lib/python3.9/site-packages uproot4
pip install --target=/dune/app/users/weishi/lib/python3.9/site-packages uproot3
pip install --target=/dune/app/users/weishi/lib/python3.9/site-packages torch
pip install --target=/dune/app/users/weishi/lib/python3.9/site-packages scipy

# Do this every time login
export PYTHONPATH=/dune/app/users/weishi/lib/python3.9/site-packages:$PYTHONPATH

python3 new_hadron_muon_mktree.py /dune/app/users/weishi/FHC.5018262.CAF.root
```

### Submit a job

If you want to submit job from ```dunegpvm*```, follow this:
```
# Make a tarball to send everything you need to run your program on grid node

tar -czvf work.tar.gz setup_NDcombEff.sh NDCAFs.txt new_hadron_muon_mktree.py muonEff30.nn muonEffModel.py

# The following long command submits your job:
# -N 2 means 2 jobs, this is now set as running 1 file per job as I have two files in txt file.
# If you have X files in your text file, set: -N X

jobsub_submit -G dune -N 2 --memory=1GB --disk=6GB --expected-lifetime=30m --cpu=1 --resource-provides=usage_model=DEDICATED,OPPORTUNISTIC,OFFSITE --tar_file_name=dropbox:///dune/app/users/weishi/test4baron/work.tar.gz --use-cvmfs-dropbox -l '+SingularityImage=\"/cvmfs/singularity.opensciencegrid.org/fermilab/fnal-wn-sl7:latest\"' --append_condor_requirements='(TARGET.HAS_Singularity==true&&TARGET.HAS_CVMFS_dune_opensciencegrid_org==true&&TARGET.HAS_CVMFS_larsoft_opensciencegrid_org==true&&TARGET.CVMFS_dune_opensciencegrid_org_REVISION>=1105&&TARGET.HAS_CVMFS_fifeuser1_opensciencegrid_org==true&&TARGET.HAS_CVMFS_fifeuser2_opensciencegrid_org==true&&TARGET.HAS_CVMFS_fifeuser3_opensciencegrid_org==true&&TARGET.HAS_CVMFS_fifeuser4_opensciencegrid_org==true)' file:///dune/app/users/weishi/test4baron/run_NDcombEff.sh
```

To query the job status: ```jobsub_q <usrname> -G dune```
To remove a job status: ```condor_rm <usrname> -G dune```

If your job gets held (HoldReasonCode 26), subcode 1 is memory, 2 is disk, 4 is too many starts (max of 10 attempts to run a job), 8 is run time. You can can also get HoldReasonCode 34 and subcode 0, which is also memory.

The job script (```run_NDcombEff.sh```) can be adjusted to run more than one file per job, if desired, contact me. The magic happens at these lines:
```
(( LINE_N = ${PROCESS} + 1 ))

for ifile in $(cat ${INPUT_TAR_DIR_LOCAL}/NDCAFs.txt | head -${LINE_N} | tail -1); do
```

## SBU nnhome machine

Make trees,
```
cd /home/wshi/repos/MuonEffNN

# Install python dependencies, default path: ~/.local/lib/python3.9/site-packages/
pip install uproot4
pip install uproot3
pip install torch

# This is not needed: export PYTHONPATH=~/.local/lib/python3.9/site-packages:$PYTHONPATH

python3 new_hadron_muon_mktree.py # Python 3.9.2
```
