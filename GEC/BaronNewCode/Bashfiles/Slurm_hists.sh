#!/bin/bash
#
#SBATCH --job-name=making_hist_files_0.001
#SBATCH --output=hist_files-%j_0.001_eff_cut_200_bins.log
#SBATCH --mail-type=END,FAIL
#SBATCH --mail-user=baron.wu@stonybrook.edu
#SBATCH --nodelist=birch # --nodes=4 --gres=gpu
#SBATCH --time=72:00:00
#!/bin/bash

cd /home/barwu/repos/DUNE_NN
root histogram_files.cpp
wait
