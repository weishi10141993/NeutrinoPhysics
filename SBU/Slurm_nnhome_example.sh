#!/bin/bash
#
#SBATCH --job-name=example
#SBATCH --output=example.log
#SBATCH --mail-type=END,FAIL
#SBATCH --mail-user=<your email>
#SBATCH --nodes=1
#SBATCH --time=01:00:00

cd <your XXX.C location>
root -l -q -b XXX.C
wait
