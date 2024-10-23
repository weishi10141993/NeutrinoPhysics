#!/bin/bash

echo "Running Marley edepsim chain"
for (( k = 10; k <= 160; ++k )); do
  ienergy=$(echo "scale=1; $k*0.5" | bc)
  echo "running $ienergy MeV"
  ./MarleyGen_MonoE_Flux_Edepsim.sh ${ienergy} 1000
  mv edep.${ienergy}.MeV.nue.Evt.1000.root edep_nue_${ienergy}MeV_1kevts.root
  mv edep_nue_${ienergy}MeV_1kevts.root /pnfs/dune/persistent/users/weishi/FD3/LArBath/Marley_Edepsim_noSecondaryDeposit/nue
done
echo "Done."
