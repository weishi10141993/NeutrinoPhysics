#!/bin/bash
#
#SBATCH --job-name=EReco
#SBATCH --output=EReco_myjobnumplaceholder.log
#SBATCH --mail-type=END,FAIL
#SBATCH --mail-user=<your-email>
#SBATCH --nodes=1
#SBATCH --time=24:00:00
#SBATCH -p long-28core

#
# Replace below with your normal environment set up
#
echo "Started set up work environment"
source /gpfs/projects/McGrewGroup/<usrname>/DUNE/ups/setup
setup mrb
setup dunetpc v09_22_02 -q e19:debug
source /gpfs/projects/McGrewGroup/<usrname>/DUNE/FDEff/localProducts_larsoft_v09_22_02_debug_e19/setup
mrbsetenv

# Go to your work directory
cd /gpfs/projects/McGrewGroup/<usrname>/DUNE/FDEff/srcs/myntuples/myntuples/MyEnergyAnalysis

echo "Started running lar"
lar -c MyEnergyAnalysis.fcl -T myntuple_myjobnumplaceholder.root -n -1 myinfileplaceholder
wait
echo "Job finished"
