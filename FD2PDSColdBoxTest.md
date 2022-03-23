# Analysis of data with CRT on top of NP02

Four runs taken with the NP02 PDS (standard arapuca modules from ProtoDUNE-I, no PoF/SoF) using the cosmic ray telescope (CRT) as a trigger. The CRT was moved around:

```
Run #   Evts    location of CRT
12451   ~6000   60 cm inside the field cage
12452   3637    190 cm inside the field cage (~center of cryostat)
12453   3900    60 cm inside the field cage
12454   3750    20 cm outside the field cage
```

The purpose is to study the effect of FC shadow effect on PDS.

Can access run files from Fermilab:

```
/pnfs/dune/tape_backed/dunepro/vd-protodune-pds/raw/2022/detector/test/None/00/01/24/51/np02_pds_run012451_0000_20220225T170210.hdf5
/pnfs/dune/tape_backed/dunepro/vd-protodune-pds/raw/2022/detector/test/None/00/01/24/52/np02_pds_run012452_0000_20220228T085242.hdf5
/pnfs/dune/tape_backed/dunepro/vd-protodune-pds/raw/2022/detector/test/None/00/01/24/53/np02_pds_run012453_0000_20220228T091757.hdf5
/pnfs/dune/tape_backed/dunepro/vd-protodune-pds/raw/2022/detector/test/None/00/01/24/54/np02_pds_run012454_0000_20220228T094438.hdf5
```

In general, to locate a file using samweb:

```
# Go to FNAL dunegpvm machine
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup dunetpc v09_22_02 -q e19:debug
setup_fnal_security
setup sam_web_client
samweb locate-file np02_pds_run012454_0000_20220228T094438.hdf5
```

To analyze the file, do the following setup (adapted from [DUNE DAQ](https://github.com/DUNE-DAQ/minidaqapp/wiki/Instructions-for-setting-up-a-v2.9.0-development-environment)):

```
ssh -X <user_name>@dunegpvm03.fnal.gov        
cd /dune/app/users/weishi
mkdir DAQ
cd DAQ

source /cvmfs/dunedaq.opensciencegrid.org/setup_dunedaq.sh
setup_dbt dunedaq-v2.9.0
dbt-create.sh -c dunedaq-v2.9.0 workdir
cd workdir

# Clone needed packages
cd sourcecode
git clone https://github.com/DUNE-DAQ/hdf5libs.git -b develop
git clone https://github.com/DUNE-DAQ/rawdatautils.git -b mman/sspdecoder
git clone https://github.com/DUNE-DAQ/dqm.git -b mman_ssp_support
git clone https://github.com/DUNE-DAQ/daqdataformats.git -b develop

# setup the work area and build the software
cd /dune/app/users/weishi/DAQ/workdir
dbt-workarea-env   
# To force reload: dbt-workarea-env --force-ups-reload
dbt-build.py
```

When you return to this work area (for example, after logging out and back in):

```
cd /dune/app/users/weishi/DAQ/workdir
source /cvmfs/dunedaq.opensciencegrid.org/setup_dunedaq.sh
setup_dbt dunedaq-v2.8.0
dbt-workarea-env
```

To run the analyzer,

```
# scp

cd /dune/app/users/weishi/DAQ/workdir/sourcecode/dqm/scripts
python ssp_processing.py

cd /dune/app/users/weishi/DAQ/workdir/sourcecode/hdf5libs/test/apps
python NP02PDSCRT_Reader.py
```

It's possible you need to set up a virtual environment to for some needed modules:

```
cd the_dir/where/you_have_your/py_script
#source /cvmfs/..../setup_dune.sh
#setup python v3_8_3b

#python -m venv .venv_v3_8_3b
python -m venv .venv_v3_8_3
source .venv_v3_8_3/bin/activate
#source .venv_v3_8_3b/bin/activate

pip install matplotlib
pip install h5py
# add other modules
```

The analyzer is adapted from PD1 [script](https://internal.dunescience.org/doxygen/SSPRawDecoder__module_8cc_source.html) starting line 275 by M. Man.

Here is a [manual](https://github.com/weishi10141993/NeutrinoPhysics/blob/main/SSP_Users_Manual_v2.05.pdf) on event readout (Ch. 11) from SiPM signal processor.

Structure:
```
GROUP "TriggerRecord000001"

  DATASET "TriggerRecordHeader"

  GROUP "PDS"
    GROUP "Region001"
      DATASET "Element01"
      DATASET "Element02"
    ...
```

# Compare with cold box test data (Dec 2021)
