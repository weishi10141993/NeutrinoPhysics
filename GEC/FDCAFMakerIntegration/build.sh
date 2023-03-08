#!/bin/bash

# Need this to enable setup below
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh

echo "Start to clone DUNE-PRISM geometric efficiency correction code"
git clone --recurse-submodules -b FD_Wei https://github.com/weishi10141993/DUNE_ND_GeoEff.git
cd DUNE_ND_GeoEff

# The following will replace the setup.sh in DUNE_ND_GeoEff
# these versions won't conflict with building duneana
setup cmake v3_24_1
setup gcc v9_3_0
setup eigen v3_4_0
setup python v3_9_2
export PYTHONPATH=${PYTHONPATH}:${PWD}/lib/

cmake -DPYTHON_EXECUTABLE:FILEPATH=`which python` .
make -j geoEff
echo "Finished compiling DUNE-PRISM geometric efficiency correction code"
