#!/bin/bash
#
#SBATCH --job-name=NuTauFit
#SBATCH --output=NuTauFit.log
#SBATCH --mail-type=END,FAIL
#SBATCH --mail-user=wei.shi.1@stonybrook.edu
#SBATCH --nodes=1
#SBATCH --time=02:00:00

cd /home/wshi/nutau/lblpwgtools/CAFAna
source build/Linux/CAFAnaEnv.sh
source /home/wshi/ROOT/root_install/bin/thisroot.sh

cd PRISM/app
PRISM_4Flavour_dChi2Scan ../../fcl/PRISM/PRISMOscScan_Grid.fcl
wait
