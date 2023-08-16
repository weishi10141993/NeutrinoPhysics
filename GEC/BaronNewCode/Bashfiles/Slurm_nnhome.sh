#!/bin/bash
#
#SBATCH --job-name=hadron_muon
#SBATCH --output=FD_maketree-%j.log
#SBATCH --mail-type=END,FAIL
#SBATCH --mail-user=baron.wu@stonybrook.edu
#SBATCH --nodelist=fir # --nodes=4 --gres=gpu
#SBATCH -c 30
#SBATCH --time=36:00:00
#!/bin/bash

cd /home/barwu/repos/DUNE_NN
python FD_maketree.py
wait
