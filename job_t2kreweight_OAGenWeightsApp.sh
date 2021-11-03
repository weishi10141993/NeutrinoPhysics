#!/bin/bash

#SBATCH --job-name=T2kReweight
#SBATCH --ntasks-per-node=24
#SBATCH --nodes=1
#SBATCH --time=24:00:00
#SBATCH -p long-24core
#SBATCH --mail-type=BEGIN,END
#SBATCH --mail-user=mo.jia@stonybrook.edu 


#source /gpfs/projects/McGrewGroup/mojia/t2ksoftware/t2kreweight/setup_reweight.sh
#source /gpfs/projects/McGrewGroup/mojia/t2ksoftware/t2kreweight/OAGenWeightsApps/build/Linux/setup.sh

#rootfile_dir=/gpfs/projects/McGrewGroup/jjjiang/my_MaCh3/MaCh3/inputs/skatm/SKMC
#output_dir=/gpfs/scratch/mojia/T2KReWeightOutput/weights
#app_dir=/gpfs/projects/McGrewGroup/mojia/t2ksoftware/t2kreweight/OAGenWeightsApps/build/Linux/bin

#filename="sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr"

#cd $output_dir
exe_dir=/gpfs/home/mojia/jobs_submission/t2kreweight

sampleID=1
while [ $sampleID -lt 20 ]; do
#    channelID=1
#    while [ $channelID -lt 13 ]; do
#        time ${app_dir}/genWeightsSKsplines_newfunctions_2021Weights -s ${rootfile_dir}/${filename}_Sample${sampleID}_Channel${channelID}.root -o ${output_dir}/${filename}_Sample${sampleID}_Channel${channelID}_T2KReWeight_Weights.root &> Sample${sampleID}_Channel${channelID}.log & 
#        let channelID+=1
#    done

    source ${exe_dir}/executeOAGenWeightApp_forSample.sh ${sampleID} &
    let sampleID+=1

done

wait 
