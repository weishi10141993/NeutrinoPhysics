#!/bin/bash
#
#SBATCH --job-name=hadron_muon
#SBATCH --output=hadron_muon.log
#SBATCH --mail-type=END,FAIL
#SBATCH --mail-user=baron.wu@stonybrook.edu
#SBATCH --nodes=1
#SBATCH --time=12:00:00

cd /home/barwu/repos/MuonEffNN
python new_hadron_muon.py 0
wait
