source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup dunetpc v09_41_00_02 -q e20:prof
setup root v6_22_08d -q e20:p392:prof
setup geant4 v4_10_6_p01e -q e20:prof
setup cmake v3_24_0
setup edepsim v3_2_0 -q e20:prof
setup duneutil v09_65_01d00 -q e20:prof
setup sam_web_client
# only need this line when doing interactively on dunegpvm
export PYTHONPATH=~/.local/lib/python3.9/site-packages:$PYTHONPATH
setup hdf5 v1_12_0b -q e20:prof

