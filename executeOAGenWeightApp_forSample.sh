#!/bin/bash

sampleID=$1

source /gpfs/projects/McGrewGroup/mojia/t2ksoftware/t2kreweight/setup_reweight.sh
source /gpfs/projects/McGrewGroup/mojia/t2ksoftware/t2kreweight/OAGenWeightsApps/build/Linux/setup.sh

rootfile_dir=/gpfs/projects/McGrewGroup/jjjiang/my_MaCh3/MaCh3/inputs/skatm/SKMC
output_dir=/gpfs/scratch/mojia/T2KReWeightOutput/weights
app_dir=/gpfs/projects/McGrewGroup/mojia/t2ksoftware/t2kreweight/OAGenWeightsApps/build/Linux/bin
#app_dir=/gpfs/projects/McGrewGroup/mojia/t2ksoftware/t2kreweight/OAGenWeightsApps/build/app/SK

filename="sk4_fcmc_tau_pcmc_ummc_fQv4r0_sf_minituple_500yr"

cd $output_dir

channelID=1
while [ $channelID -lt 13 ]; do
     time ${app_dir}/genWeights_T2KSKAtm_OA2020_AdlerAngle_MatrixElement_SIPN_CRPA -s ${rootfile_dir}/${filename}_Sample${sampleID}_Channel${channelID}.root -o ${output_dir}/${filename}_Sample${sampleID}_Channel${channelID}_T2KReWeight_Weights.root &> Sample${sampleID}_Channel${channelID}.log  
     let channelID+=1

done


