MAINDIR=/pnfs/dune/scratch/users/weishi

SEARCHDIR=DebugFluxFit/EVisReco_prism_default/flux_Nov17_0/NDFHC

FILENAME=ND_FHC.State.*

for file in {0..749}; do

        if [ ! -f ${MAINDIR}/${SEARCHDIR}/${FILENAME}.${file}.root ]; then
                echo "re-run ${FILENAME}.${file}.root"
        fi

done
