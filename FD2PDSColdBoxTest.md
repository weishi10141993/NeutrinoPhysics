# Analysis of data with CRT on top of NP02

Four runs taken with the NP02 PDS using the cosmic ray telescope (CRT) as a trigger. The CRT was moved around:

```
Run #   Evts    location of CRT
12451   ~6000   60 cm inside the field cage
12452   3637    190 cm inside the field cage (~center of cryostat)
12453   3900    60 cm inside the field cage
12454   3750    20 cm outside the field cage
```

Can access from Fermilab:

```
/pnfs/dune/tape_backed/dunepro/vd-protodune-pds/raw/2022/detector/test/None/00/01/24/51/np02_pds_run012451_0000_20220225T170210.hdf5
/pnfs/dune/tape_backed/dunepro/vd-protodune-pds/raw/2022/detector/test/None/00/01/24/52/np02_pds_run012452_0000_20220228T085242.hdf5
/pnfs/dune/tape_backed/dunepro/vd-protodune-pds/raw/2022/detector/test/None/00/01/24/53/np02_pds_run012453_0000_20220228T091757.hdf5
/pnfs/dune/tape_backed/dunepro/vd-protodune-pds/raw/2022/detector/test/None/00/01/24/54/np02_pds_run012454_0000_20220228T094438.hdf5
```

In general, to locate a file using samweb:

```
# Go to fnal machine
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup dunetpc v09_22_02 -q e19:debug
setup_fnal_security
setup sam_web_client
samweb locate-file np02_pds_run012454_0000_20220228T094438.hdf5
```

# Compare with cold box test data (Dec 2021)
